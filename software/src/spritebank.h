// Sprite, bitmap and music data. Definitions live in spritebank.c.
//
// Compression schemes and their byte costs are documented next to the data
// they apply to.

#pragma once

#include <stdint.h>

// =================== TETRIS ===================
extern const uint8_t H_Grid_TTTRIS[];
extern const uint8_t Pieces_TTRIS[];
extern const uint8_t tiny_PREVIEW_block_TTTRIS[];
extern const uint8_t tinyblock_TTTRIS[];
extern const uint8_t tinyblock2_TTTRIS[];
extern const uint8_t police_TTRIS[];
extern const uint8_t start_button_1_TTRIS[];
extern const uint8_t start_button_2_TTRIS[];
extern const uint8_t chateau_TTRIS[];
extern const uint8_t BACKGROUND_TTRIS[];

// =================== PACMAN ===================
typedef struct PERSONAGE
{
  uint8_t anim;
  uint8_t guber;
  uint8_t type;
  uint8_t DirectionAnim;
  uint8_t DirectionH;
  uint8_t DirectionV;
  uint8_t x;
  uint8_t Decalagey;
  int8_t y;
  uint8_t switchanim;
} PERSONAGE;

extern const uint8_t Music[];
extern const uint8_t fruits[];
extern const uint8_t caracters[];
extern const uint8_t back[];
extern const uint8_t DOTS_ROW[9];
extern const uint8_t DOTS[];
extern const uint8_t BB_DELTA_ROW[9];
extern const uint8_t BB_DELTA[];

// =================== SHARED ===================
extern const uint8_t ANNOYEDMILK_RLE[];

// =================== TEST MODE ===================
extern const uint16_t JOYTEST_ROW[9];
extern const uint8_t JOYTEST_RLE[];
extern const uint8_t Joy[];
extern const uint8_t boutton[];
