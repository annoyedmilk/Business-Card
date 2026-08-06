#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// ch32fun configuration for the Business Card.
// Full list of options: ch32fun/ch32fun/ch32fun.h

// ---------------------------------------------------------------------------
// Clock
// ---------------------------------------------------------------------------
// 12 MHz: HSI with the AHB prescaler at /2. SystemInit() leaves it at /1, so
// JOY_init() sets RCC->CFGR0 = RCC_HPRE_DIV2 straight afterwards.
//
// This must match what RCC actually runs at. Delay_Ms, Delay_Us, the frame
// pacing and the I2C bus clock all derive from it, so a mismatch scales every
// timing in the firmware by the same factor.
//
// Chosen for battery life: core current scales roughly with clock. Game speed
// does not depend on this value, frame_sync() in driver.h pins it, but the
// clock does have to be fast enough to finish a frame inside its budget. The
// I2C rate is unaffected and still divides exactly: CCR = 12 / (3 * 1) = 4.
#define FUNCONF_USE_HSI           1
#define FUNCONF_USE_PLL           0
#define FUNCONF_SYSTEM_CORE_CLOCK 12000000

// SysTick must be fed from HCLK, not the HCLK/8 that ch32fun defaults to.
// DELAY_US_TIME is FUNCONF_SYSTEM_CORE_CLOCK/8000000 under that divider,
// which truncates to 3 here and would quantise Delay_Us() to 333 ns steps.
// The buzzer forms its tone around Delay_Us(255 - freq) and needs the
// resolution. With HCLK the tick is 12 MHz, giving DELAY_US_TIME 12 and
// DELAY_MS_TIME 12000. The frame pacing and the sleeps count the same ticks.
#define FUNCONF_SYSTICK_USE_HCLK  1

// ---------------------------------------------------------------------------
// Size
// ---------------------------------------------------------------------------
// No room in 16K for printf or the hardfault logger; both pull in formatting
// code and the debug transport.
#define FUNCONF_USE_DEBUGPRINTF   0
#define FUNCONF_USE_UARTPRINTF    0
#define FUNCONF_DEBUG_HARDFAULT   0

#endif
