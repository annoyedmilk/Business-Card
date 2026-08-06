// ===================================================================================
// Tiny Joypad Drivers for CH32V003                                           * v1.1 *
// ===================================================================================
//
// MCU abstraction layer, over ch32fun (https://github.com/cnlohr/ch32fun).
// 2023 by Stefan Wagner: https://github.com/wagiminator
// 2024 by annoyedmilk: https://github.com/annoyedmilk

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include "ch32fun.h"
#include "oled_min.h"
#include "spritebank.h"

// Game states
#define MENU_STATE 0
#define TETRIS_STATE 1
#define PACMAN_STATE 2
#define TEST_STATE 3

// Menu entries, in display order. The sound toggle is last.
#define MENU_TETRIS 0
#define MENU_PACMAN 1
#define MENU_TEST 2
#define MENU_SOUND 3
#define MENU_LAST MENU_SOUND

// Pin assignments. The OLED sits on PC1 (SDA) and PC2 (SCL); i2c_tx.c owns
// those two directly.
#define PIN_ACT PA2  // pin connected to fire button
#define PIN_BEEP PA1 // pin connected to buzzer
#define PIN_PAD PC4  // pin conected to direction buttons

// PC4 is ADC channel 2 (see the CH32V003 pin alternate function table)
#define PIN_PAD_ANALOG ANALOG_2

// ===================================================================================
// Pin and timing primitives
// ===================================================================================
// The game code names these; ch32fun spells them differently.
#define DLY_ms(n) Delay_Ms(n)
#define DLY_us(n) Delay_Us(n)
#define PIN_low(pin) funDigitalWrite((pin), FUN_LOW)
#define PIN_high(pin) funDigitalWrite((pin), FUN_HIGH)
#define PIN_read(pin) funDigitalRead(pin)
#define ADC_read() ((uint16_t)funAnalogRead(PIN_PAD_ANALOG))

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

// ===================================================================================
// Frame pacing
// ===================================================================================
// Both games come from a 16 MHz ATtiny85 and carry no frame timing of their
// own: one pass through the game loop is one step of the game, so unpaced,
// game speed is just however fast the hardware runs.
//
// frame_sync() holds each pass to a fixed wall-clock period, which keeps game
// speed independent of the system clock and of how long a frame took to draw.
//
// The two periods differ because the games do different work per pass: Pacman
// redraws every pass, Tetris every seventh.
//
// These are calibration values, not figures measured from an ATtiny. Tune
// them until the games feel right; larger is slower.
#define FRAME_US_PACMAN 12000 // Pacman: one full redraw per pass
#define FRAME_US_TETRIS 2000  // Tetris: 7 passes per redraw

  // Deadline for the current frame, in free-running SysTick ticks.
  static uint32_t frame_deadline;

  // Wait until the current frame's time budget is used up, then arm the next.
  static inline void frame_sync(uint32_t period_us)
  {
    uint32_t period = period_us * DELAY_US_TIME;
    // If we overran by more than a whole period, give up on catching up and
    // resynchronise, otherwise a slow frame makes the next few run flat out.
    if ((int32_t)(SysTick->CNT - frame_deadline) > (int32_t)period)
      frame_deadline = SysTick->CNT;
    else
      while ((int32_t)(SysTick->CNT - frame_deadline) < 0)
        ;
    frame_deadline += period;
  }

  static inline void frame_start(uint32_t period_us)
  {
    frame_deadline = SysTick->CNT + period_us * DELAY_US_TIME;
  }

// Kept for the original call site at the bottom of the Pacman loop.
#define JOY_SLOWDOWN() frame_sync(FRAME_US_PACMAN)

// OLED
#define JOY_OLED_end I2C_stop
#define JOY_OLED_send(b) I2C_write(b)
#define JOY_OLED_data_start(y) \
  {                            \
    OLED_setpos(0, y);         \
    OLED_data_start();         \
  }

// Buttons
#define JOY_act_pressed() (!PIN_read(PIN_ACT))
#define JOY_act_released() (PIN_read(PIN_ACT))

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
    SystemInit(); // HSI, no PLL, no AHB division: 24 MHz

    funGpioInitAll();

    funPinMode(PIN_PAD, GPIO_CFGLR_IN_ANALOG);
    funPinMode(PIN_ACT, GPIO_CFGLR_IN_PUPD);
    PIN_high(PIN_ACT); // input pull-up: direction is chosen by the output latch
    funPinMode(PIN_BEEP, GPIO_CFGLR_OUT_10Mhz_PP);
    PIN_high(PIN_BEEP);

    funAnalogInit();

    // Shortest sample time. The JOY_* thresholds above are calibrated against
    // it; funAnalogInit() leaves every channel on a longer one.
    ADC1->SAMPTR1 = 0;
    ADC1->SAMPTR2 = 0;

    OLED_init();
  }

  static void show_startup_logo(void)
  {
    // Left half stays white, right half carries the 64x64 logo. This covers
    // every page and column, so the screen does not need clearing first.
    const uint8_t *p = ANNOYEDMILK_RLE;
    uint8_t run = 0, val = 0;

    for (uint8_t y = 0; y < 8; y++)
    {
      JOY_OLED_data_start(y);
      for (uint8_t x = 0; x < 64; x++)
        JOY_OLED_send(0xFF);
      for (uint8_t x = 0; x < 64; x++)
      {
        if (!run) // next run-length pair
        {
          run = *p++;
          val = *p++;
        }
        run--;
        JOY_OLED_send(val);
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

// ===================================================================================
// Menu text
// ===================================================================================
// 5 columns per glyph, one bit per pixel, LSB = topmost pixel. Only the
// characters the menu actually uses are here.
#define GL_SP 0
#define GL_COL 1
#define GL_A 2
#define GL_C 3
#define GL_D 4
#define GL_E 5
#define GL_F 6
#define GL_I 7
#define GL_M 8
#define GL_N 9
#define GL_O 10
#define GL_P 11
#define GL_R 12
#define GL_S 13
#define GL_T 14
#define GL_U 15
#define GL_CUR 16

  static const uint8_t FONT5[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, // space
      0x00, 0x36, 0x36, 0x00, 0x00, // :
      0x7E, 0x11, 0x11, 0x11, 0x7E, // A
      0x3E, 0x41, 0x41, 0x41, 0x22, // C
      0x7F, 0x41, 0x41, 0x41, 0x3E, // D
      0x7F, 0x49, 0x49, 0x49, 0x41, // E
      0x7F, 0x09, 0x09, 0x09, 0x01, // F
      0x00, 0x41, 0x7F, 0x41, 0x00, // I
      0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
      0x7F, 0x04, 0x08, 0x10, 0x7F, // N
      0x3E, 0x41, 0x41, 0x41, 0x3E, // O
      0x7F, 0x09, 0x09, 0x09, 0x06, // P
      0x7F, 0x09, 0x19, 0x29, 0x46, // R
      0x46, 0x49, 0x49, 0x49, 0x31, // S
      0x01, 0x01, 0x7F, 0x01, 0x01, // T
      0x3F, 0x40, 0x40, 0x40, 0x3F, // U
      0x00, 0x41, 0x22, 0x14, 0x08, // > (menu cursor)
  };

  static const uint8_t STR_TETRIS[] = {GL_T, GL_E, GL_T, GL_R, GL_I, GL_S};
  static const uint8_t STR_PACMAN[] = {GL_P, GL_A, GL_C, GL_M, GL_A, GL_N};
  static const uint8_t STR_TEST[] = {GL_T, GL_E, GL_S, GL_T, GL_SP, GL_SP};
  // The trailing space on ON is what erases the second F of OFF.
  static const uint8_t STR_SND_ON[] = {GL_S, GL_O, GL_U, GL_N, GL_D, GL_COL, GL_O, GL_N, GL_SP};
  static const uint8_t STR_SND_OFF[] = {GL_S, GL_O, GL_U, GL_N, GL_D, GL_COL, GL_O, GL_F, GL_F};

  // Screen page each menu entry is drawn on.
  static const uint8_t MENU_PAGE[] = {1, 3, 5, 7};

  // Draw glyphs on a page, inverted, after `lead` blank columns.
  static void draw_glyphs(uint8_t page, const uint8_t *str, uint8_t n, uint8_t lead)
  {
    JOY_OLED_data_start(page);
    while (lead--)
      JOY_OLED_send(~0x00);
    while (n--)
    {
      const uint8_t *g = &FONT5[(*str++) * 5];
      for (uint8_t i = 0; i < 5; i++)
        JOY_OLED_send(~*g++);
      JOY_OLED_send(~0x00); // inter-character gap
    }
    JOY_OLED_end();
  }

  static inline void show_menu(void)
  {
    draw_glyphs(MENU_PAGE[MENU_TETRIS], STR_TETRIS, 6, 6);
    draw_glyphs(MENU_PAGE[MENU_PACMAN], STR_PACMAN, 6, 6);
    draw_glyphs(MENU_PAGE[MENU_TEST], STR_TEST, 6, 6);
    draw_glyphs(MENU_PAGE[MENU_SOUND], sound_enabled ? STR_SND_ON : STR_SND_OFF, 9, 6);
  }

  // Menu selection, and the selection the screen currently shows. 255 means
  // the menu needs repainting; game exit paths set it via menu_invalidate()
  // so the next handle_menu() pass redraws both text and cursor.
  static uint8_t menu_position = 0;
  static uint8_t menu_drawn = 255;
#define menu_invalidate() (menu_drawn = 255)

  static inline void handle_menu(uint8_t *game_state, uint8_t *current_game)
  {
    bool needsRedraw = false;

    // Handle menu navigation
    if (JOY_up_pressed() && menu_position > 0)
    {
      menu_position--;
      if (sound_enabled)
        needsRedraw = true;
      DLY_ms(200);
    }

    if (JOY_down_pressed() && menu_position < MENU_LAST)
    {
      menu_position++;
      if (sound_enabled)
        needsRedraw = true;
      DLY_ms(200);
    }

    if (JOY_act_pressed())
    {
      // Toggle before the confirmation tone, so switching sound on is audible
      // and switching it off is silent.
      if (menu_position == MENU_SOUND)
      {
        sound_enabled = !sound_enabled;
        needsRedraw = true;
      }

      if (sound_enabled)
        JOY_sound(20, 150);
      JOY_sound(100, 150);
      DLY_ms(200);

      if (menu_position != MENU_SOUND)
      {
        *current_game = menu_position;
        *game_state = (menu_position == MENU_TETRIS)   ? TETRIS_STATE
                      : (menu_position == MENU_PACMAN) ? PACMAN_STATE
                                                       : TEST_STATE;
      }
    }

    // Only redraw if position changed or update needed
    if (needsRedraw || menu_drawn != menu_position)
    {
      show_menu();

      // Cursor for the current selection. The glyph occupies columns 0-4,
      // inside the 6 blank columns the text is indented by, so it cannot
      // collide with a letter.
      static const uint8_t STR_CUR[] = {GL_CUR};
      draw_glyphs(MENU_PAGE[menu_position], STR_CUR, 1, 0);

      menu_drawn = menu_position;
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
      return;
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
  static uint16_t rnval = 0xACE1;
  uint16_t JOY_random(void)
  {
    rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
    return rnval;
  }

// Delays
#define JOY_DLY_ms DLY_ms

  long map(long x, long in_min, long in_max, long out_min, long out_max)
  {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }

#ifdef __cplusplus
};
#endif
