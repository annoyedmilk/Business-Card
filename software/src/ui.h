// ===================================================================================
// Menu, startup logo and jingle
// ===================================================================================

#pragma once

#include <stdint.h>

// Game states
#define MENU_STATE 0
#define TETRIS_STATE 1
#define PACMAN_STATE 2
#define TEST_STATE 3

// Fills the screen: left half white, right half the 64x64 logo. Covers every
// page and column, so it doubles as the screen clear on the way back to the
// menu.
void show_startup_logo(void);

void play_start_jingle(void);

// Advances the menu by one pad reading and starts a game on the action
// button. Repaints only when the selection or the sound setting changed.
void handle_menu(uint8_t *game_state);

// Forces the next handle_menu() to repaint text and cursor. Game exit paths
// call this because they have overwritten the screen.
void menu_invalidate(void);
