// ===================================================================================
// Joypad test mode
// ===================================================================================
// Port of the TinyJoypad TESTMODE tool by Daniel C.

#include "testmode.h"

#include "input.h"
#include "oled_min.h"
#include "sprite.h"
#include "spritebank.h"
#include "timing.h"
#include "ui.h"

// Joypad test mode
// ===================================================================================
// Port of the TinyJoypad TESTMODE tool by Daniel C. Draws a fixed test screen
// with a marker that follows the direction pad and a button indicator that
// lights while the action button is held.
//
// Hold the action button for TEST_EXIT_FRAMES consecutive frames to leave.
#define TEST_EXIT_FRAMES 50

static uint8_t joytest_row[128];
static uint8_t joytest_row_y = 0xFF;

// Expand one run-length encoded row of the test screen.
static uint8_t joytest_at(uint8_t x, uint8_t y)
{
  if (y != joytest_row_y)
  {
    uint8_t p = 0;
    for (uint16_t i = JOYTEST_ROW[y]; i < JOYTEST_ROW[y + 1]; i++)
    {
      uint8_t run = JOYTEST_RLE[i * 2];
      uint8_t val = JOYTEST_RLE[i * 2 + 1];
      while (run--)
        joytest_row[p++] = val;
    }
    joytest_row_y = y;
  }
  return joytest_row[x];
}

void handle_test_mode(uint8_t *state)
{
  uint8_t x_joy = 29, y_joy = 30;
  uint8_t held = 0;

  // Rising sweep, same as the original, doubles as a buzzer check.
  for (uint8_t t = 2; t < 250; t++)
    JOY_sound(t, 1);

  frame_start(FRAME_US_PACMAN);

  while (*state == TEST_STATE)
  {
    uint8_t act = JOY_act_pressed();
    uint8_t pad = JOY_pad();

    // The marker springs back to centre when nothing is pressed, so a stuck
    // reading shows up as a marker that will not return. Both axes come from
    // one sample, so diagonals move the marker diagonally.
    if (pad & PAD_RIGHT)
      x_joy = (x_joy < 47) ? x_joy + 6 : 47;
    else if (pad & PAD_LEFT)
      x_joy = (x_joy > 11) ? x_joy - 6 : 11;
    else
    {
      if (x_joy < 29)
        x_joy += 6;
      if (x_joy > 29)
        x_joy -= 6;
    }

    if (pad & PAD_DOWN)
      y_joy = (y_joy < 48) ? y_joy + 6 : 48;
    else if (pad & PAD_UP)
      y_joy = (y_joy > 12) ? y_joy - 6 : 12;
    else
    {
      if (y_joy < 30)
        y_joy += 6;
      if (y_joy > 30)
        y_joy -= 6;
    }

    for (uint8_t y = 0; y < 8; y++)
    {
      OLED_page_start(y);
      for (uint8_t x = 0; x < 128; x++)
      {
        uint8_t marker = sprite_blit(x_joy, y_joy, x, y, 0, Joy);
        uint8_t button = act ? (0xFF - sprite_blit(97, 20, x, y, 0, boutton)) : 0xFF;
        OLED_send(marker | (joytest_at(x, y) & button));
      }
      OLED_end();
    }

    held = act ? held + 1 : 0;
    if (held >= TEST_EXIT_FRAMES)
    {
      JOY_sound(100, 150);
      while (JOY_act_pressed()) // do not carry the press into the menu
        ;
      show_startup_logo();
      menu_invalidate();
      *state = MENU_STATE;
    }

    frame_sync(FRAME_US_PACMAN);
  }
}
