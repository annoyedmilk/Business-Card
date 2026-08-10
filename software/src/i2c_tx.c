// ===================================================================================
// Basic I2C Master Functions (write only) for CH32V003                       * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator
//
// Setup sequence follows ch32fun's ssd1306_i2c.h, which is known to drive this
// class of SSD1306/SSD1315 panel from a CH32V003.
//
// Every wait is bounded. An unresponsive panel used to hang the firmware in
// one of these loops with the screen dark and nothing else running; now the
// transfer is abandoned and the rest of the board still works.

#include "i2c_tx.h"

// Roughly 30 ms at 24 MHz, far longer than any byte needs.
#define I2C_TIMEOUT 100000

// Event codes, STAR1 in the low half and STAR2 in the high half
#define I2C_EVT_START 0x00030001       // BUSY, MSL, SB
#define I2C_EVT_ADDR 0x00070082        // BUSY, MSL, ADDR, TXE, TRA
#define I2C_EVT_TRANSMITTED 0x00070084 // BUSY, MSL, TXE, BTF, TRA

// Read order matters: STAR1 before STAR2.
static uint8_t I2C_checkEvent(uint32_t mask)
{
  uint32_t status = I2C1->STAR1 | (I2C1->STAR2 << 16);
  return (status & mask) == mask;
}

// Init I2C
void I2C_init(void)
{
  // Enable GPIO port C and I2C module
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  RCC->APB1PCENR |= RCC_I2C1EN;

  // PC1 (SDA) and PC2 (SCL): alternate function, open drain, 10 MHz
  funPinMode(PC1, GPIO_CFGLR_OUT_10Mhz_AF_OD);
  funPinMode(PC2, GPIO_CFGLR_OUT_10Mhz_AF_OD);

  // Reset the peripheral so every register starts from a known state
  RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
  RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;

  // Input clock, in MHz
  I2C1->CTLR2 = (FUNCONF_SYSTEM_CORE_CLOCK / 2000000) & I2C_CTLR2_FREQ;

  // Bus clock. Fast mode, 33% duty: CCR = clk / (3 * rate). The 16:9 duty
  // option is not usable here, as clk / (25 * rate) truncates to zero.
  I2C1->CKCFGR = ((FUNCONF_SYSTEM_CORE_CLOCK / (3 * I2C_CLKRATE)) & I2C_CKCFGR_CCR) | I2C_CKCFGR_FS;

  // Enable I2C and acknowledging
  I2C1->CTLR1 |= I2C_CTLR1_PE;
  I2C1->CTLR1 |= I2C_CTLR1_ACK;
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr)
{
  int32_t timeout;

  timeout = I2C_TIMEOUT;
  while ((I2C1->STAR2 & I2C_STAR2_BUSY) && timeout--)
    ; // wait until bus ready
  if (timeout < 0)
    return;

  I2C1->CTLR1 |= I2C_CTLR1_START; // set START condition

  timeout = I2C_TIMEOUT;
  while (!I2C_checkEvent(I2C_EVT_START) && timeout--)
    ; // wait for START generated
  if (timeout < 0)
    return;

  I2C1->DATAR = addr; // send slave address + R/W bit

  timeout = I2C_TIMEOUT;
  while (!I2C_checkEvent(I2C_EVT_ADDR) && timeout--)
    ; // wait for address transmitted
}

// Send data byte via I2C bus
void I2C_write(uint8_t data)
{
  int32_t timeout = I2C_TIMEOUT;
  while (!(I2C1->STAR1 & I2C_STAR1_TXE) && timeout--)
    ; // wait for last byte transmitted
  if (timeout < 0)
    return;
  I2C1->DATAR = data; // send data byte
}

// Stop I2C transmission
void I2C_stop(void)
{
  int32_t timeout = I2C_TIMEOUT;
  while (!I2C_checkEvent(I2C_EVT_TRANSMITTED) && timeout--)
    ;                            // wait for last byte transmitted
  I2C1->CTLR1 |= I2C_CTLR1_STOP; // set STOP condition
}
