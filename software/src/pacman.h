// Tiny Pacman.

#pragma once

#include <stdint.h>

// Runs Tiny Pacman until *state changes. Returns to the menu when the last
// life is lost.
void handle_pacman_game(uint8_t *state);
