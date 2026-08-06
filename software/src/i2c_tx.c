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

// Loop count, not a time. Far longer than any byte or block needs.
#define I2C_TIMEOUT 100000

// Event codes, STAR1 in the low half and STAR2 in the high half
#define I2C_EVT_START 0x00030001    // BUSY, MSL, SB
#define I2C_EVT_ADDR 0x00070082     // BUSY, MSL, ADDR, TXE, TRA
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

  // Set pin PC1 (SDA) and PC2 (SCL) to output, open-drain, 10MHz, multiplex
  GPIOC->CFGLR = (GPIOC->CFGLR & ~(((uint32_t)0b1111 << (1 << 2)) | ((uint32_t)0b1111 << (2 << 2)))) | (((uint32_t)0b1101 << (1 << 2)) | ((uint32_t)0b1101 << (2 << 2)));

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

  // Pixel data goes out by DMA, see I2C_write_block()
  RCC->AHBPCENR |= RCC_DMA1EN;
  I2C1->CTLR2 |= I2C_CTLR2_DMAEN;
  DMA1_Channel6->PADDR = (uint32_t)&I2C1->DATAR;
}

// Push a block out over DMA1 channel 6, the I2C1 TX channel.
//
// A full page is 128 bytes, about 1.2 ms on the wire. Spinning on TXE for
// that is what the byte-at-a-time path does; here the core sleeps instead,
// which is most of a frame once all eight pages are counted.
void I2C_write_block(const uint8_t *buf, uint16_t len)
{
  int32_t timeout = I2C_TIMEOUT;

  DMA1_Channel6->CFGR = 0; // disable before reprogramming
  DMA1_Channel6->MADDR = (uint32_t)buf;
  DMA1_Channel6->CNTR = len;
  DMA1->INTFCR = DMA1_Channel6_IT_Mask;
  DMA1_Channel6->CFGR = DMA_CFGR1_DIR    // memory -> peripheral
                        | DMA_CFGR1_MINC // walk the buffer
                        | DMA_CFGR1_PL_1 // high priority
                        | DMA_CFGR1_EN;

  while (!(DMA1->INTFR & DMA_TCIF6) && timeout > 0)
  {
    sleep_us(200);
    timeout -= 200;
  }

  DMA1_Channel6->CFGR = 0;
  DMA1->INTFCR = DMA1_Channel6_IT_Mask;
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
