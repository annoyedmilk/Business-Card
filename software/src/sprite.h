// ===================================================================================
// Sprite blitter
// ===================================================================================
// Shared by both games and by test mode. The panel is addressed in 8-pixel
// pages, so a sprite at an arbitrary y straddles two pages and every fetch is
// split across the page boundary.
//
// Sprite format: width, height, then one byte per column per row. A sprite may
// hold several frames back to back, selected by the FRAME argument.

#pragma once

#include <stdint.h>

// Page containing pixel row y, and the offset of y within that page.
static inline uint8_t sprite_page_of(uint8_t y) { return y >> 3; }
static inline uint8_t sprite_shift_of(uint8_t y) { return y & 7; }

// The half of a source byte that falls in the upper (up != 0) or lower page.
static inline uint8_t sprite_split(uint8_t shift, uint8_t input, uint8_t up)
{
  return up ? (uint8_t)(input << shift) : (uint8_t)(input >> (8 - shift));
}

// One byte of the sprite as it appears at column xPASS of page yPASS, or 0
// where the sprite does not cover that byte.
uint8_t sprite_blit(int8_t xPos, int8_t yPos, uint8_t xPASS, uint8_t yPASS,
                    uint8_t FRAME, const uint8_t *SPRITES);
