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
// Game speed does not depend on this value; frame_sync() in timing.c pins it.
// The clock is set for rendering headroom, and costs power accordingly: core
// current scales roughly with it.
#define FUNCONF_USE_HSI           1
#define FUNCONF_USE_PLL           0
#define FUNCONF_SYSTEM_CORE_CLOCK 24000000

// Feed SysTick from HCLK rather than the HCLK/8 ch32fun defaults to, so
// DELAY_US_TIME is FUNCONF_SYSTEM_CORE_CLOCK/1000000 (24) instead of
// .../8000000 (3). The frame pacing counts the same ticks.
//
// At 24 MHz both settings happen to keep perfect time, because 24/8 divides
// exactly. The point is that they stop doing so as soon as the clock moves:
// at 12 MHz the HCLK/8 form truncates 12/8 to 1, and every delay, tone and
// frame period then runs 33% fast. Deriving the tick straight from HCLK
// keeps Delay_Us(), the buzzer and frame_sync() honest at any clock.
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
