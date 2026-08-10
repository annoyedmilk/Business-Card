// Hardware test mode, for board bring-up.

#pragma once

#include <stdint.h>

// Draws the test screen until the action button is held long enough to exit.
void handle_test_mode(uint8_t *state);
