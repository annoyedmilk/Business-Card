// ===================================================================================
// Menu, startup logo and jingle
// ===================================================================================
// 2024 by annoyedmilk: https://github.com/annoyedmilk

#include "ui.h"

#include "input.h"
#include "oled_min.h"
#include "spritebank.h"

// Menu entries, in display order. The sound toggle is last.
#define MENU_TETRIS 0
#define MENU_PACMAN 1
#define MENU_TEST 2
#define MENU_SOUND 3
#define MENU_LAST MENU_SOUND

void show_startup_logo(void)
{
  const uint8_t *p = ANNOYEDMILK_RLE;
  uint8_t run = 0, val = 0;

  for (uint8_t y = 0; y < 8; y++)
  {
    OLED_page_start(y);
    for (uint8_t x = 0; x < 64; x++)
      OLED_send(0xFF);
    for (uint8_t x = 0; x < 64; x++)
    {
      if (!run) // next run-length pair
      {
        run = *p++;
        val = *p++;
      }
      run--;
      OLED_send(val);
    }
    OLED_end();
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
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // space
    0x00,
    0x36,
    0x36,
    0x00,
    0x00, // :
    0x7E,
    0x11,
    0x11,
    0x11,
    0x7E, // A
    0x3E,
    0x41,
    0x41,
    0x41,
    0x22, // C
    0x7F,
    0x41,
    0x41,
    0x41,
    0x3E, // D
    0x7F,
    0x49,
    0x49,
    0x49,
    0x41, // E
    0x7F,
    0x09,
    0x09,
    0x09,
    0x01, // F
    0x00,
    0x41,
    0x7F,
    0x41,
    0x00, // I
    0x7F,
    0x02,
    0x0C,
    0x02,
    0x7F, // M
    0x7F,
    0x04,
    0x08,
    0x10,
    0x7F, // N
    0x3E,
    0x41,
    0x41,
    0x41,
    0x3E, // O
    0x7F,
    0x09,
    0x09,
    0x09,
    0x06, // P
    0x7F,
    0x09,
    0x19,
    0x29,
    0x46, // R
    0x46,
    0x49,
    0x49,
    0x49,
    0x31, // S
    0x01,
    0x01,
    0x7F,
    0x01,
    0x01, // T
    0x3F,
    0x40,
    0x40,
    0x40,
    0x3F, // U
    0x00,
    0x41,
    0x22,
    0x14,
    0x08, // > (menu cursor)
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
  OLED_page_start(page);
  while (lead--)
    OLED_send(~0x00);
  while (n--)
  {
    const uint8_t *g = &FONT5[(*str++) * 5];
    for (uint8_t i = 0; i < 5; i++)
      OLED_send(~*g++);
    OLED_send(~0x00); // inter-character gap
  }
  OLED_end();
}

static void show_menu(void)
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
void menu_invalidate(void) { menu_drawn = 255; }

void handle_menu(uint8_t *game_state)
{
  uint8_t needsRedraw = 0;
  uint8_t pad = JOY_pad();

  // Handle menu navigation
  if ((pad & PAD_UP) && menu_position > 0)
  {
    menu_position--;
    if (sound_enabled)
      needsRedraw = 1;
    DLY_ms(200);
  }

  if ((pad & PAD_DOWN) && menu_position < MENU_LAST)
  {
    menu_position++;
    if (sound_enabled)
      needsRedraw = 1;
    DLY_ms(200);
  }

  if (JOY_act_pressed())
  {
    // Toggle before the confirmation tone, so switching sound on is audible
    // and switching it off is silent.
    if (menu_position == MENU_SOUND)
    {
      sound_enabled = !sound_enabled;
      needsRedraw = 1;
    }

    if (sound_enabled)
      JOY_sound(20, 150);
    JOY_sound(100, 150);
    DLY_ms(200);

    if (menu_position != MENU_SOUND)
    {
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
