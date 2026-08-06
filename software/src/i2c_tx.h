// ===================================================================================
// Basic I2C Master Functions (write only) for CH32V003                       * v1.0 *
// ===================================================================================
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator
//
// Write only: the OLED is never read back. Pins are fixed to PC1 (SDA) and
// PC2 (SCL), which is how the board is wired. External pull-ups are required.

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "ch32fun.h"

// I2C bus clock rate.
//
// The display is what limits frame rate: a full 1024-byte frame is 9216 bit
// periods, 9.2 ms at this rate. 400 kHz would cost 23 ms and cap the games
// near 43 fps.
//
// 1 MHz is past what the datasheet promises, but it is what ch32fun uses by
// default for this class of panel. At 12 MHz it also divides exactly:
// CCR = 12 / (3 * 1) = 4. Drop to 400000 if a panel misbehaves.
#define I2C_CLKRATE 1000000

#define STK_CTLR_STIE (1 << 1) // SysTick interrupt enable

    // Idle in __WFI() for up to `us`, waking on the SysTick compare or any
    // other interrupt. The caller must re-check whatever it is waiting for.
    //
    // The compare is armed relative to a freshly read CNT so it is always in
    // the future. Arming it at a time already past would set no flag and the
    // core would sleep until the counter wrapped, around six minutes.
    // SysTick_Handler() in driver.h clears the flag.
    static inline void sleep_us(uint32_t us)
    {
        SysTick->CMP = SysTick->CNT + us * DELAY_US_TIME;
        SysTick->SR = 0;
        SysTick->CTLR |= STK_CTLR_STIE;
        __WFI();
        SysTick->CTLR &= ~STK_CTLR_STIE;
    }

    void I2C_init(void);          // I2C init function
    void I2C_start(uint8_t addr); // I2C start transmission, addr must contain R/W bit
    void I2C_write(uint8_t data); // I2C transmit one data byte via I2C
    void I2C_stop(void);          // I2C stop transmission

    // Hand a block to DMA1 channel 6 and return once it has all been pushed
    // into the peripheral. Sleeps while it waits.
    void I2C_write_block(const uint8_t *buf, uint16_t len);

#ifdef __cplusplus
};
#endif
