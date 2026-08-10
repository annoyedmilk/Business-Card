// Tiny Tris. Everything else about the game, including the high-score record
// in flash, is private to tetris.c.

#pragma once

#include <stdint.h>

// Runs Tiny Tris until *state changes. Holding DOWN on entry clears the
// stored high score.
void handle_tetris_game(uint8_t *state);
