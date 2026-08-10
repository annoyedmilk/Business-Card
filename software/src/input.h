// ===================================================================================
// Buttons, direction pad and buzzer
// ===================================================================================
// 2023 by Stefan Wagner: https://github.com/wagiminator
// 2024 by annoyedmilk: https://github.com/annoyedmilk

#pragma once

#include <stdint.h>
#include "ch32fun.h"

// Pin assignments. The OLED sits on PC1 (SDA) and PC2 (SCL); i2c_tx.c owns
// those two directly.
#define PIN_ACT PA2  // fire button
#define PIN_BEEP PA1 // buzzer
#define PIN_PAD PC4  // direction buttons

// PC4 is ADC channel 2 (see the CH32V003 pin alternate function table)
#define PIN_PAD_ANALOG ANALOG_2

// The game code names these; ch32fun spells them differently.
#define DLY_ms(n) Delay_Ms(n)
#define DLY_us(n) Delay_Us(n)
#define PIN_low(pin) funDigitalWrite((pin), FUN_LOW)
#define PIN_high(pin) funDigitalWrite((pin), FUN_HIGH)
#define PIN_read(pin) funDigitalRead(pin)

// Direction pad, as a bitmask. All four directions come from one conversion,
// so callers testing more than one must take a single sample and test that.
#define PAD_UP 0x01
#define PAD_DOWN 0x02
#define PAD_LEFT 0x04
#define PAD_RIGHT 0x08

#define JOY_act_pressed() (!PIN_read(PIN_ACT))
#define JOY_act_released() (PIN_read(PIN_ACT))

extern uint8_t sound_enabled; // 1 for on, 0 for off

void JOY_init(void);
uint8_t JOY_pad(void);
void JOY_sound(uint8_t freq, uint8_t dur);
uint16_t JOY_random(void);
