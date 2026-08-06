#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// ch32fun configuration for the Business Card.
// Full list of options: ch32fun/ch32fun/ch32fun.h

// ---------------------------------------------------------------------------
// Clock
// ---------------------------------------------------------------------------
// 24 MHz: full HSI, no PLL, no AHB division, which is what SystemInit()
// programs by default.
//
// This must match what RCC actually runs at. Delay_Ms, Delay_Us, the frame
// pacing and the I2C bus clock all derive from it, so a mismatch scales every
// timing in the firmware by the same factor.
//
// Game speed does not depend on this value; frame_sync() in driver.h pins it.
// The clock is set for rendering headroom, and costs power accordingly: core
// current scales roughly with it.
#define FUNCONF_USE_HSI           1
#define FUNCONF_USE_PLL           0
#define FUNCONF_SYSTEM_CORE_CLOCK 24000000

// SysTick must be fed from HCLK, not the HCLK/8 that ch32fun defaults to.
// DELAY_US_TIME is FUNCONF_SYSTEM_CORE_CLOCK/8000000 under that divider,
// which truncates to 3 here and would quantise Delay_Us() to 333 ns steps.
// The buzzer forms its tone around Delay_Us(255 - freq) and needs the
// resolution. With HCLK the tick is 24 MHz, giving DELAY_US_TIME 24 and
// DELAY_MS_TIME 24000. The frame pacing counts the same ticks.
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
