// ===================================================================================
// Basic I2C Master Functions (write only) for CH32V003                       * v1.0 *
// ===================================================================================
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator
//
// Write only: the OLED is never read back. Pins are fixed to PC1 (SDA) and
// PC2 (SCL), which is how the board is wired. External pull-ups are required.

#pragma once

#include "ch32fun.h"

// I2C bus clock rate.
//
// The display is what limits frame rate: a full 1024-byte frame is 9216 bit
// periods, 9.2 ms at this rate. 400 kHz would cost 23 ms and cap the games
// near 43 fps.
//
// 1 MHz is past what the datasheet promises, but it is what ch32fun uses by
// default for this class of panel. At 24 MHz it also divides exactly:
// CCR = 24 / (3 * 1) = 8. Drop to 400000 if a panel misbehaves.
#define I2C_CLKRATE 1000000

void I2C_init(void);          // I2C init function
void I2C_start(uint8_t addr); // I2C start transmission, addr must contain R/W bit
void I2C_write(uint8_t data); // I2C transmit one data byte via I2C
void I2C_stop(void);          // I2C stop transmission
