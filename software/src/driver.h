// ===================================================================================
// Tiny Joypad Drivers for CH32V003                                           * v1.1 *
// ===================================================================================
//
// MCU abstraction layer.
// 2023 by Stefan Wagner: https://github.com/wagiminator
// 2024 by annoyedmilk: https://github.com/annoyedmilk

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include "system.h"
#include "gpio.h"
#include "oled_min.h"
#include "spritebank.h"

// Game states
#define MENU_STATE 0
#define TETRIS_STATE 1
#define PACMAN_STATE 2

// Pin assignments
#define PIN_ACT PA2  // pin connected to fire button
#define PIN_BEEP PA1 // pin connected to buzzer
#define PIN_PAD PC4  // pin conected to direction buttons
#define PIN_SCL PC2  // pin connected to OLED (I2C SCL)
#define PIN_SDA PC1  // pin connected to OLED (I2C SDA)

// Joypad calibration values
#define JOY_N 197  // joypad UP
#define JOY_NE 259 // joypad UP + RIGHT
#define JOY_E 90   // joypad RIGHT
#define JOY_SE 388 // joypad DOWN + RIGHT
#define JOY_S 346  // joypad DOWN
#define JOY_SW 616 // joypad DOWN + LEFT
#define JOY_W 511  // joypad LEFT
#define JOY_NW 567 // JOYPAD UP + LEFT
#define JOY_DEV 20 // deviation

// Sound enable
#define JOY_SOUND 1 // 0: no sound, 1: with sound

// Game slow-down delay
#define JOY_SLOWDOWN() // DLY_ms(10)

// OLED commands
#define JOY_OLED_init OLED_init
#define JOY_OLED_end I2C_stop
#define JOY_OLED_send(b) I2C_write(b)
#define JOY_OLED_send_command(c) OLED_send_command(c)
#define JOY_OLED_data_start(y) \
  {                            \
    OLED_setpos(0, y);         \
    OLED_data_start();         \
  }

// Buttons
#define JOY_act_pressed() (!PIN_read(PIN_ACT))
#define JOY_act_released() (PIN_read(PIN_ACT))
#define JOY_pad_pressed() (ADC_read() > 10)
#define JOY_pad_released() (ADC_read() <= 10)
#define JOY_all_released() (JOY_act_released() && JOY_pad_released())

  // Function declarations
  static inline uint8_t JOY_up_pressed(void);
  static inline uint8_t JOY_down_pressed(void);
  static inline uint8_t JOY_left_pressed(void);
  static inline uint8_t JOY_right_pressed(void);
  void JOY_sound(uint8_t freq, uint8_t dur);
  uint16_t JOY_random(void);
  static inline void JOY_init(void);
  static inline void show_menu(void);
  static void show_startup_logo(void);
  void play_start_jingle(void);
  static inline void handle_menu(uint8_t *game_state, uint8_t *current_game);
  extern uint8_t sound_enabled; // 1 for on, 0 for off

  // Implementation of declared functions
  static inline void JOY_init(void)
  {
    PIN_input_AN(PIN_PAD);
    PIN_input_PU(PIN_ACT);
    PIN_output(PIN_BEEP);
    PIN_high(PIN_BEEP);
    OLED_init();
    ADC_init();
    ADC_input(PIN_PAD);
  }

  static void show_startup_logo(void)
  {
    // Fill entire screen with white first
    for (uint8_t y = 0; y < 8; y++)
    {
      JOY_OLED_data_start(y);
      for (uint8_t x = 0; x < 128; x++)
      {
        JOY_OLED_send(0xFF); // White background (all pixels on)
      }
      JOY_OLED_end();
    }

    // Draw 64x64 logo starting at x=64 (right half of screen)
    for (uint8_t y = 0; y < 8; y++)
    {
      JOY_OLED_data_start(y);
      // First 64 pixels remain white
      for (uint8_t x = 0; x < 64; x++)
      {
        JOY_OLED_send(0xFF);
      }
      // Draw 64 pixels of logo
      for (uint8_t x = 0; x < 64; x++)
      {
        JOY_OLED_send(ANNOYEDMILK[x + (y * 64)]);
      }
      JOY_OLED_end();
    }
  }

  void play_start_jingle(void)
  {
    uint8_t notes[] = {20, 60, 100};
    uint8_t durations[] = {150, 150, 150};

    for (uint8_t i = 0; i < 3; i++)
    {
      JOY_sound(notes[i], durations[i]);
      DLY_ms(100);
    }
    DLY_ms(100);
  }

  static inline void show_menu(void)
  {
    // Draw "TETRIS" at line 1
    JOY_OLED_data_start(1);

    // Initial space (inverted)
    for (uint8_t i = 0; i < 6; i++)
    {
      JOY_OLED_send(~0x00);
    }

    // T (inverted)
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x00);

    // E (inverted)
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x00);

    // T (inverted)
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x01);
    JOY_OLED_send(~0x00);

    // R (inverted)
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x09);
    JOY_OLED_send(~0x19);
    JOY_OLED_send(~0x29);
    JOY_OLED_send(~0x46);
    JOY_OLED_send(~0x00);

    // I (inverted)
    JOY_OLED_send(~0x00);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x00);
    JOY_OLED_send(~0x00);

    // S (inverted)
    JOY_OLED_send(~0x46);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x31);
    JOY_OLED_end();

    // Draw "PACMAN" at line 3
    JOY_OLED_data_start(3);

    // Initial space (inverted)
    for (uint8_t i = 0; i < 6; i++)
    {
      JOY_OLED_send(~0x00);
    }

    // P (inverted)
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x09);
    JOY_OLED_send(~0x09);
    JOY_OLED_send(~0x09);
    JOY_OLED_send(~0x06);
    JOY_OLED_send(~0x00);

    // A (inverted)
    JOY_OLED_send(~0x7E);
    JOY_OLED_send(~0x11);
    JOY_OLED_send(~0x11);
    JOY_OLED_send(~0x11);
    JOY_OLED_send(~0x7E);
    JOY_OLED_send(~0x00);

    // C (inverted)
    JOY_OLED_send(~0x3E);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x22);
    JOY_OLED_send(~0x00);

    // M (inverted)
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x02);
    JOY_OLED_send(~0x0C);
    JOY_OLED_send(~0x02);
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x00);

    // A (inverted)
    JOY_OLED_send(~0x7E);
    JOY_OLED_send(~0x11);
    JOY_OLED_send(~0x11);
    JOY_OLED_send(~0x11);
    JOY_OLED_send(~0x7E);
    JOY_OLED_send(~0x00);

    // N (inverted)
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x04);
    JOY_OLED_send(~0x08);
    JOY_OLED_send(~0x10);
    JOY_OLED_send(~0x7F);
    JOY_OLED_end();

    // Draw "SOUND: ON/OFF" at line 6
    JOY_OLED_data_start(6);

    // Initial space (inverted)
    for (uint8_t i = 0; i < 6; i++)
    {
      JOY_OLED_send(~0x00);
    }

    // S (inverted)
    JOY_OLED_send(~0x46);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x49);
    JOY_OLED_send(~0x31);
    JOY_OLED_send(~0x00);

    // O (inverted)
    JOY_OLED_send(~0x3E);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x3E);
    JOY_OLED_send(~0x00);

    // U (inverted)
    JOY_OLED_send(~0x3F);
    JOY_OLED_send(~0x40);
    JOY_OLED_send(~0x40);
    JOY_OLED_send(~0x40);
    JOY_OLED_send(~0x3F);
    JOY_OLED_send(~0x00);

    // N (inverted)
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x04);
    JOY_OLED_send(~0x08);
    JOY_OLED_send(~0x10);
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x00);

    // D (inverted)
    JOY_OLED_send(~0x7F);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x41);
    JOY_OLED_send(~0x3E);
    JOY_OLED_send(~0x00);

    // : (inverted)
    JOY_OLED_send(~0x00);
    JOY_OLED_send(~0x36);
    JOY_OLED_send(~0x36);
    JOY_OLED_send(~0x00);
    JOY_OLED_send(~0x00);
    JOY_OLED_send(~0x00);

    // Display ON or OFF based on sound_enabled
    if (sound_enabled)
    {
      // ON
      // O (inverted)
      JOY_OLED_send(~0x3E);
      JOY_OLED_send(~0x41);
      JOY_OLED_send(~0x41);
      JOY_OLED_send(~0x41);
      JOY_OLED_send(~0x3E);
      JOY_OLED_send(~0x00);

      // N (inverted)
      JOY_OLED_send(~0x7F);
      JOY_OLED_send(~0x04);
      JOY_OLED_send(~0x08);
      JOY_OLED_send(~0x10);
      JOY_OLED_send(~0x7F);
      JOY_OLED_send(~0x00);

      // Extra spaces to clear the second F from OFF
      JOY_OLED_send(~0x00);
      JOY_OLED_send(~0x00);
      JOY_OLED_send(~0x00);
      JOY_OLED_send(~0x00);
      JOY_OLED_send(~0x00);
      JOY_OLED_send(~0x00);
    }
    else
    {
      // OFF
      // O (inverted)
      JOY_OLED_send(~0x3E);
      JOY_OLED_send(~0x41);
      JOY_OLED_send(~0x41);
      JOY_OLED_send(~0x41);
      JOY_OLED_send(~0x3E);
      JOY_OLED_send(~0x00);

      // F (inverted)
      JOY_OLED_send(~0x7F);
      JOY_OLED_send(~0x09);
      JOY_OLED_send(~0x09);
      JOY_OLED_send(~0x09);
      JOY_OLED_send(~0x01);
      JOY_OLED_send(~0x00);

      // F (inverted)
      JOY_OLED_send(~0x7F);
      JOY_OLED_send(~0x09);
      JOY_OLED_send(~0x09);
      JOY_OLED_send(~0x09);
      JOY_OLED_send(~0x01);
      JOY_OLED_send(~0x00);
    }
    JOY_OLED_end();
  }

  static inline void handle_menu(uint8_t *game_state, uint8_t *current_game)
  {
    static uint8_t menu_position = 0;
    static uint8_t prev_position = 255; // Initialize to invalid position to force first draw
    bool needsRedraw = false;

    // Handle menu navigation
    if (JOY_up_pressed() && menu_position > 0)
    {
      menu_position--;
      if (sound_enabled)
        needsRedraw = true;
      DLY_ms(200);
    }

    if (JOY_down_pressed() && menu_position < 2)
    {
      menu_position++;
      if (sound_enabled)
        needsRedraw = true;
      DLY_ms(200);
    }

    if (JOY_act_pressed())
    {
      if (menu_position < 2)
      {
        if (sound_enabled)
          JOY_sound(20, 150);
        JOY_sound(100, 150);
        DLY_ms(200);
        *current_game = menu_position;
        *game_state = (menu_position == 0) ? TETRIS_STATE : PACMAN_STATE;
      }
      else
      {
        sound_enabled = !sound_enabled;
        if (sound_enabled)
          JOY_sound(20, 150);
        JOY_sound(100, 150);
        needsRedraw = true;
        DLY_ms(200);
      }
    }

    // Only redraw if position changed or update needed
    if (needsRedraw || prev_position != menu_position)
    {
      show_menu();

      // Draw cursor for current selection (adjusted for new menu layout)
      JOY_OLED_data_start(menu_position == 0 ? 1 : menu_position == 1 ? 3
                                                                      : 6);

      // Draw inverted '>' cursor bitmap
      JOY_OLED_send(~0x00); // Inverted space
      JOY_OLED_send(~0x41); // Inverted >
      JOY_OLED_send(~0x22);
      JOY_OLED_send(~0x14);
      JOY_OLED_send(~0x08);
      JOY_OLED_end();

      prev_position = menu_position;
    }
  }

  static inline uint8_t JOY_up_pressed(void)
  {
    uint16_t val = ADC_read();
    return (((val > JOY_N - JOY_DEV) && (val < JOY_N + JOY_DEV)) | ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV)) | ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV)));
  }

  static inline uint8_t JOY_down_pressed(void)
  {
    uint16_t val = ADC_read();
    return (((val > JOY_S - JOY_DEV) && (val < JOY_S + JOY_DEV)) | ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV)) | ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV)));
  }

  static inline uint8_t JOY_left_pressed(void)
  {
    uint16_t val = ADC_read();
    return (((val > JOY_W - JOY_DEV) && (val < JOY_W + JOY_DEV)) | ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV)) | ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV)));
  }

  static inline uint8_t JOY_right_pressed(void)
  {
    uint16_t val = ADC_read();
    return (((val > JOY_E - JOY_DEV) && (val < JOY_E + JOY_DEV)) | ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV)) | ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV)));
  }

  // Buzzer
  void JOY_sound(uint8_t freq, uint8_t dur)
  {
    if (!sound_enabled)
      return; // Add this line
    while (dur--)
    {
#if JOY_SOUND == 1
      if (freq)
        PIN_low(PIN_BEEP);
#endif
      DLY_us(255 - freq);
      PIN_high(PIN_BEEP);
      DLY_us(255 - freq);
    }
  }

  // Pseudo random number generator
  uint16_t rnval = 0xACE1;
  uint16_t JOY_random(void)
  {
    rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
    return rnval;
  }

// Delays
#define JOY_DLY_ms DLY_ms
#define JOY_DLY_us DLY_us

// Additional Defines
#define abs(n) ((n >= 0) ? (n) : (-(n)))

  long map(long x, long in_min, long in_max, long out_min, long out_max)
  {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

#ifdef __cplusplus
};
#endif
