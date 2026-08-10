// ===================================================================================
// Tiny Tris
// ===================================================================================
//         >>>>>  T-I-N-Y  T-R-I-S v3 for ATTINY85  GPL v3 <<<<<
//                     Programmer: Daniel C 2019-2021
//              Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                      https://WWW.TINYJOYPAD.COM
//
// Modified to work with CH32V003 by Stefan Wagner: https://github.com/wagiminator
// Modified to include both Tiny Tris and Tiny Pacman by annoyedmilk
// ===================================================================================

#include "tetris.h"

#include "ch32fun.h"
#include "input.h"
#include "oled_min.h"
#include "sprite.h"
#include "spritebank.h"
#include "timing.h"
#include "ui.h"

// Global Variables TETRIS
// ===================================================================================
static uint8_t Grid_TTRIS[12][3] = {{0}};
static const uint8_t MEM_TTTRIS[16] = {0, 2, 0, 4, 3, 7, 6, 9, 9, 12, 11, 15, 14, 17, 17, 19};
static uint8_t Level_TTRIS;
static uint16_t Scores_TTRIS;
static uint16_t Nb_of_line_F_TTRIS;
static uint8_t Level_Speed_ADJ_TTRIS;
static uint8_t Nb_of_line_TTRIS[3];
static uint8_t Score_digits_TTRIS[5]; // score, least significant digit first
static uint8_t Level_digits_TTRIS[2]; // level, least significant digit first
static uint8_t RND_VAR_TTRIS;
static uint8_t LONG_PRESS_X_TTRIS;
static uint8_t DOWN_DESACTIVE_TTRIS;
static uint8_t DROP_SPEED_TTRIS;
static uint8_t SPEED_x_trig_TTRIS;
static uint8_t DROP_TRIG_TTRIS;
static int8_t xx_TTRIS, yy_TTRIS;
static uint8_t Piece_Mat2_TTRIS[5][5];
static uint8_t Ripple_filter_TTRIS;
static uint8_t PIECEs_TTRIS;
static uint8_t PIECEs_TTRIS_PREVIEW;
static uint8_t PIECEs_rot_TTRIS;
static uint8_t DROP_BREAK_TTRIS;
static int8_t OU_SUIS_JE_X_TTRIS;
static int8_t OU_SUIS_JE_Y_TTRIS;
static uint8_t OU_SUIS_JE_X_ENGAGED_TTRIS;
static uint8_t OU_SUIS_JE_Y_ENGAGED_TTRIS;
static int8_t DEPLACEMENT_XX_TTRIS;
static int8_t DEPLACEMENT_YY_TTRIS;

// ===================================================================================
// Function Prototypes TETRIS
// ===================================================================================
void reset_Score_TTRIS(void);
uint8_t PSEUDO_RND_TTRIS(void);
void SND_TTRIS(uint8_t Snd_TTRIS);
void INTRO_MANIFEST_TTRIS(void);
void END_DROP_TTRIS(void);
void SETUP_NEW_PREVIEW_PIECE_TTRIS(uint8_t *Rot_TTRIS);
void CONTROLE_TTRIS(uint8_t *Rot_TTRIS);
void Game_Play_TTRIS(void);
uint8_t End_Play_TTRIS(void);
void DELETE_LINE_TTRIS(void);
uint8_t Calcul_of_Score_TTRIS(uint8_t Tmp_TTRIS);
void FLASH_LINE_TTRIS(uint8_t *PASS_LINE);
void PAINT_LINE_TTRIS(uint8_t VISIBLE, uint8_t *PASS_LINE);
void Clean_Grid_TTRIS(uint8_t *PASS_LINE);
uint8_t CHECK_if_Rot_Ok_TTRIS(uint8_t *Rot_TTRIS);
uint8_t Check_collision_x_TTRIS(int8_t x_Axe);
uint8_t Check_collision_y_TTRIS(int8_t y_Axe);
void Move_Piece_TTRIS(void);
void Ou_suis_Je_TTRIS(int8_t xx_, int8_t yy_);
void Select_Piece_TTRIS(uint8_t Piece_);
void rotate_Matrix_TTRIS(uint8_t ROT);
uint8_t Scan_Piece_Matrix_TTRIS(int8_t x_Mat, int8_t y_Mat);
uint8_t GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN);
uint8_t CHANGE_GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN, uint8_t VALUE);
uint8_t sprite_blit(int8_t xPos, int8_t yPos, uint8_t xPASS, uint8_t yPASS, uint8_t FRAME, const uint8_t *SPRITES);
uint8_t H_grid_Scan_TTRIS(uint8_t xPASS);
uint8_t Recupe_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t NEXT_BLOCK_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t RECUPE_BACKGROUND_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t DropPiece_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t sprite_page_of(uint8_t Valeur);
uint8_t sprite_shift_of(uint8_t Valeur);
void Tiny_Flip_TTRIS(uint8_t HR_TTRIS);
void Flip_intro_TTRIS(uint8_t *TIMER1);
uint8_t intro_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1);
uint8_t Recupe_Start_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1);
uint8_t recupe_Chateau_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t recupe_SCORES_TTRIS(uint8_t xPASS, uint8_t yPASS);
void Convert_Nb_of_line_TTRIS(void);
void Convert_Score_TTRIS(void);
uint8_t recupe_Nb_of_line_TTRIS(uint8_t xPASS, uint8_t yPASS);
uint8_t recupe_LEVEL_TTRIS(uint8_t xPASS, uint8_t yPASS);
void INIT_ALL_VAR_TTRIS(void);
void recupe_HIGHSCORE_TTRIS(void);
void Reset_Value_TTRIS(void);
void save_HIGHSCORE_TTRIS(void);
void Check_NEW_RECORD(void);

void handle_tetris_game(uint8_t *state)
{
  Reset_Value_TTRIS();

  // As on the original: holding DOWN while Tetris starts clears the stored
  // high score (the globals were just zeroed, so this saves zeros).
  if (JOY_pad() & PAD_DOWN)
  {
    DLY_ms(1000);
    if (JOY_pad() & PAD_DOWN)
      save_HIGHSCORE_TTRIS();
  }

  while (*state == TETRIS_STATE)
  {
    uint8_t Rot_TTRIS = 0;
    uint8_t SKIP_FRAME = 0;

    // Initialize game
    INIT_ALL_VAR_TTRIS();
    Game_Play_TTRIS();
    Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
    INTRO_MANIFEST_TTRIS();
    SETUP_NEW_PREVIEW_PIECE_TTRIS(&Rot_TTRIS);
    Tiny_Flip_TTRIS(128);
    DLY_ms(1000);

    // Set initial position
    xx_TTRIS = 55;
    yy_TTRIS = 5;
    frame_start(FRAME_US_TETRIS);

    // Game loop
    while (*state == TETRIS_STATE)
    {

      CONTROLE_TTRIS(&Rot_TTRIS);

      if (DROP_BREAK_TTRIS == 15)
      {
        END_DROP_TTRIS();
        if (End_Play_TTRIS())
        {
          Tiny_Flip_TTRIS(128);
          SND_TTRIS(3);
          DLY_ms(2000);
          Check_NEW_RECORD();
          break;
        }
        yy_TTRIS = 2;
        xx_TTRIS = 55;
        PIECEs_TTRIS = PIECEs_TTRIS_PREVIEW;
        SETUP_NEW_PREVIEW_PIECE_TTRIS(&Rot_TTRIS);
        DOWN_DESACTIVE_TTRIS = 1;
        Tiny_Flip_TTRIS(128);
        Game_Play_TTRIS();
        Tiny_Flip_TTRIS(128);
      }

      if ((JOY_act_pressed()) && (Ripple_filter_TTRIS == 0))
      {
        PSEUDO_RND_TTRIS();
        Ripple_filter_TTRIS = 1;
      }

      Move_Piece_TTRIS();
      if (SKIP_FRAME == 6)
      {
        Tiny_Flip_TTRIS(82);
        SKIP_FRAME = 0;
      }
      else
      {
        SKIP_FRAME++;
      }

      frame_sync(FRAME_US_TETRIS);
    }
  }
}

// ===================================================================================
// Functions TETRIS
// ===================================================================================
void reset_Score_TTRIS(void)
{
  for (uint8_t x = 0; x < 3; x++)
  {
    Nb_of_line_TTRIS[x] = 0;
  }
  Level_TTRIS = 0;
  Scores_TTRIS = 0;
  Nb_of_line_F_TTRIS = 0;
}

uint8_t PSEUDO_RND_TTRIS(void)
{
  RND_VAR_TTRIS = (RND_VAR_TTRIS < 6) ? RND_VAR_TTRIS + 1 : 0;
  return RND_VAR_TTRIS;
}

void SND_TTRIS(uint8_t Snd_TTRIS)
{
  uint8_t q;
  switch (Snd_TTRIS)
  {
  case 0:
    JOY_sound(3, 5);
    JOY_sound(10, 10);
    JOY_sound(3, 5);
    break;
  case 1:
    JOY_sound(3, 2);
    break;
  case 2:
    for (q = 1; q < 10; q++)
    {
      JOY_sound(40, 80);
      JOY_sound(150, 80);
    }
    break;
  case 3:
    for (q = 100; q > 10; q--)
    {
      JOY_sound(q * 2, 6);
      JOY_sound(q, 12);
    }
    break;
  case 4:
    JOY_sound(20, 150);
    JOY_sound(100, 150);
    break;
  case 5:
    for (q = 0; q < 126; q++)
    {
      JOY_sound(q * 2, 1);
    }
    break;
  default:
    break;
  }
}

void INTRO_MANIFEST_TTRIS(void)
{
  uint8_t TIMER_1 = 0;
  recupe_HIGHSCORE_TTRIS();
  Convert_Nb_of_line_TTRIS();
  Flip_intro_TTRIS(&TIMER_1);
  while (1)
  {
    PIECEs_TTRIS = PSEUDO_RND_TTRIS();
    if (JOY_act_pressed())
    {
      reset_Score_TTRIS();
      break;
    }
    DLY_ms(33);
    TIMER_1 = (TIMER_1 < 7) ? TIMER_1 + 1 : 0;
    Flip_intro_TTRIS(&TIMER_1);
  }
  SND_TTRIS(4);
}

void END_DROP_TTRIS(void)
{
  uint8_t x, y;
  DROP_BREAK_TTRIS = 0;
  for (y = 0; y < 5; y++)
  {
    for (x = 0; x < 5; x++)
    {
      if (Piece_Mat2_TTRIS[x][y] == 1)
      {
        CHANGE_GRID_STAT_TTRIS(OU_SUIS_JE_X_TTRIS + (x), OU_SUIS_JE_Y_TTRIS + (y), 1);
      }
    }
  }
  Scores_TTRIS = (OU_SUIS_JE_Y_TTRIS < 9) ? Scores_TTRIS + 2 : Scores_TTRIS + 1;
  yy_TTRIS = 0;
  xx_TTRIS = 0;
  DELETE_LINE_TTRIS();
  Convert_Nb_of_line_TTRIS();
}

void SETUP_NEW_PREVIEW_PIECE_TTRIS(uint8_t *Rot_TTRIS)
{
  PIECEs_TTRIS_PREVIEW = PSEUDO_RND_TTRIS();
  Select_Piece_TTRIS(PIECEs_TTRIS);
  *Rot_TTRIS = 0;
  rotate_Matrix_TTRIS(*Rot_TTRIS);
}

void CONTROLE_TTRIS(uint8_t *Rot_TTRIS)
{
  const uint8_t pad = JOY_pad();

  if ((OU_SUIS_JE_X_ENGAGED_TTRIS == 0))
  {
    if (SPEED_x_trig_TTRIS == 0)
    {
      if (pad & PAD_RIGHT)
      {
        if (LONG_PRESS_X_TTRIS == 0)
        {
          SND_TTRIS(1);
        }
        if ((LONG_PRESS_X_TTRIS == 0) || (LONG_PRESS_X_TTRIS == 20))
        {
          DEPLACEMENT_XX_TTRIS = 1;
          SPEED_x_trig_TTRIS = 2;
        }
        if (LONG_PRESS_X_TTRIS < 20)
        {
          LONG_PRESS_X_TTRIS++;
        }
      }
      if (pad & PAD_LEFT)
      {
        if (LONG_PRESS_X_TTRIS == 0)
        {
          SND_TTRIS(1);
        }
        if ((LONG_PRESS_X_TTRIS == 0) || (LONG_PRESS_X_TTRIS == 20))
        {
          DEPLACEMENT_XX_TTRIS = -1;
          SPEED_x_trig_TTRIS = 2;
        }
        if (LONG_PRESS_X_TTRIS < 20)
        {
          LONG_PRESS_X_TTRIS++;
        }
      }
    }
    else
    {
      SPEED_x_trig_TTRIS = (SPEED_x_trig_TTRIS > 0) ? SPEED_x_trig_TTRIS - 1 : 0;
    }
  }
  if ((pad & (PAD_LEFT | PAD_RIGHT)) == 0)
  {
    LONG_PRESS_X_TTRIS = 0;
    PSEUDO_RND_TTRIS();
  }

  if (JOY_act_released())
  {
    if ((OU_SUIS_JE_X_ENGAGED_TTRIS == 0) && (OU_SUIS_JE_Y_ENGAGED_TTRIS == 0))
    {
      Ripple_filter_TTRIS = 0;
    }
  }
  if ((Ripple_filter_TTRIS == 1))
  {
    CHECK_if_Rot_Ok_TTRIS(Rot_TTRIS);
    Ripple_filter_TTRIS = 2;
  }
  if (OU_SUIS_JE_Y_ENGAGED_TTRIS == 0)
  {
    DROP_TRIG_TTRIS--;
    if (DROP_TRIG_TTRIS == 0)
    {
      DEPLACEMENT_YY_TTRIS = 1;
      DROP_TRIG_TTRIS = Level_Speed_ADJ_TTRIS;
    }
  }
  if (DROP_SPEED_TTRIS > 0)
  {
    DROP_SPEED_TTRIS--;
  }
  else
  {
    DROP_SPEED_TTRIS = Level_Speed_ADJ_TTRIS;
  }
  if (pad & PAD_DOWN)
  {

    // ajouter cest 2 ligne
    if (OU_SUIS_JE_X_ENGAGED_TTRIS == 0)
    {
      DEPLACEMENT_XX_TTRIS = 0;
      LONG_PRESS_X_TTRIS = 1;
    } // ajouter cest 2 ligne

    PSEUDO_RND_TTRIS();
    if (DOWN_DESACTIVE_TTRIS == 0)
    {
      DEPLACEMENT_YY_TTRIS = 1;
      DROP_SPEED_TTRIS = 0;
    }
  }
  else
  {
    DOWN_DESACTIVE_TTRIS = 0;
  }
}

void Game_Play_TTRIS(void)
{
  uint8_t LEVEL_TMP = (Nb_of_line_F_TTRIS / 20);
  if (Level_TTRIS != LEVEL_TMP)
  {
    Level_TTRIS = LEVEL_TMP;
    SND_TTRIS(2);
  }
  if (Level_TTRIS < 21)
  {
    // Level 0..20 maps linearly onto a drop delay of 11..1. Written out
    // rather than via a generic map(): RV32EC has no multiply or divide, so
    // the general form costs two libcalls for a halving.
    Level_Speed_ADJ_TTRIS = 11 - (Level_TTRIS >> 1);
  }
}

uint8_t End_Play_TTRIS(void)
{
  for (uint8_t t = 0; t < 12; t++)
  {
    if (GRID_STAT_TTRIS(t, 1) == 1)
    {
      return 1;
    }
  }
  return 0;
}

void DELETE_LINE_TTRIS(void)
{
  uint8_t LOOP, SCAN_Line, ADDs_Block, OK_DELETE = 0;
  uint8_t LINE_MEM[19] = {0};
  uint8_t Nb_of_Line_temp = 0;
  for (LOOP = 0; LOOP < 19; LOOP++)
  {
    ADDs_Block = 1;
    for (SCAN_Line = 0; SCAN_Line < 12; SCAN_Line++)
    {
      if (GRID_STAT_TTRIS(SCAN_Line, LOOP) == 0)
      {
        ADDs_Block = 0;
      }
    }
    if (ADDs_Block)
    {
      LINE_MEM[LOOP] = 1;
      OK_DELETE = 1;
    }
  }
  if (OK_DELETE == 1)
  {
    FLASH_LINE_TTRIS(&LINE_MEM[0]);
    Clean_Grid_TTRIS(&LINE_MEM[0]);
  }
  for (LOOP = 0; LOOP < 19; LOOP++)
  {
    if (LINE_MEM[LOOP] == 1)
    {
      Nb_of_Line_temp++;
    }
  }
  Nb_of_line_F_TTRIS = Nb_of_line_F_TTRIS + Nb_of_Line_temp;
  Scores_TTRIS = (Scores_TTRIS + Calcul_of_Score_TTRIS(Nb_of_Line_temp));
}

uint8_t Calcul_of_Score_TTRIS(uint8_t Tmp_TTRIS)
{
  switch (Tmp_TTRIS)
  {
  case 0:
    return 0;
    break;
  case 1:
    return 2;
    break;
  case 2:
    return 5;
    break;
  case 3:
    return 8;
    break;
  case 4:
    return 12;
    break;
  default:
    return 0;
    break;
  }
}

void FLASH_LINE_TTRIS(uint8_t *PASS_LINE)
{
  uint8_t LOOP;
  for (LOOP = 0; LOOP < 5; LOOP++)
  {
    PAINT_LINE_TTRIS(1, &PASS_LINE[0]);
    Tiny_Flip_TTRIS(82);

    PAINT_LINE_TTRIS(0, &PASS_LINE[0]);
    Tiny_Flip_TTRIS(82);
  }
  SND_TTRIS(5);
}

void PAINT_LINE_TTRIS(uint8_t VISIBLE, uint8_t *PASS_LINE)
{
  uint8_t LOOP, SCAN_Line;
  for (LOOP = 0; LOOP < 19; LOOP++)
  {
    if (PASS_LINE[LOOP] == 1)
    {
      for (SCAN_Line = 0; SCAN_Line < 12; SCAN_Line++)
      {
        CHANGE_GRID_STAT_TTRIS(SCAN_Line, LOOP, VISIBLE);
      }
    }
  }
}

void Clean_Grid_TTRIS(uint8_t *PASS_LINE)
{
  uint8_t GRID_2 = 18, GRID_1 = 18;
  uint8_t x;
  while (1)
  {
    if (PASS_LINE[GRID_1] == 1)
    {
      GRID_2 = GRID_1;
      GRID_2 = (GRID_2 > 0) ? GRID_2 - 1 : 0;
      break;
    }
    GRID_1 = (GRID_1 > 0) ? GRID_1 - 1 : 0;
  }
  while (1)
  {
    while (1)
    {
      if (PASS_LINE[GRID_2] == 1)
      {
        GRID_2 = (GRID_2 > 0) ? GRID_2 - 1 : 0;
      }
      else
      {
        break;
      }
    }
    for (x = 0; x < 12; x++)
    {
      CHANGE_GRID_STAT_TTRIS(x, GRID_1, (GRID_2 > 0) ? GRID_STAT_TTRIS(x, GRID_2) : 0);
    }
    GRID_1 = (GRID_1 > 0) ? GRID_1 - 1 : 0;
    GRID_2 = (GRID_2 > 0) ? GRID_2 - 1 : 0;
    if (GRID_1 == 0)
    {
      break;
    }
  }
}

uint8_t CHECK_if_Rot_Ok_TTRIS(uint8_t *Rot_TTRIS)
{
  uint8_t Mem_rot = *Rot_TTRIS;
  Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
  *Rot_TTRIS = (*Rot_TTRIS < PIECEs_rot_TTRIS) ? *Rot_TTRIS + 1 : 0;
  rotate_Matrix_TTRIS(*Rot_TTRIS);

  if ((Check_collision_x_TTRIS(OU_SUIS_JE_X_ENGAGED_TTRIS) || (Check_collision_y_TTRIS(OU_SUIS_JE_Y_ENGAGED_TTRIS))) != 0)
  {
    *Rot_TTRIS = Mem_rot;
    rotate_Matrix_TTRIS(*Rot_TTRIS);
    return 1;
  }
  SND_TTRIS(0);
  return 0;
}

uint8_t Check_collision_x_TTRIS(int8_t x_Axe)
{
  uint8_t x, y;
  for (y = 0; y < 5; y++)
  {
    for (x = 0; x < 5; x++)
    {
      if (Piece_Mat2_TTRIS[x][y] == 1)
      {
        if (GRID_STAT_TTRIS((x + OU_SUIS_JE_X_TTRIS) + x_Axe, y + OU_SUIS_JE_Y_TTRIS))
        {
          return 1;
        }
      }
    }
  }
  return 0;
}

uint8_t Check_collision_y_TTRIS(int8_t y_Axe)
{
  uint8_t x, y;
  for (y = 0; y < 5; y++)
  {
    for (x = 0; x < 5; x++)
    {
      if (Piece_Mat2_TTRIS[x][y] == 1)
      {
        if (GRID_STAT_TTRIS(x + OU_SUIS_JE_X_TTRIS, (y + OU_SUIS_JE_Y_TTRIS) + y_Axe))
        {
          return 1;
        }
      }
    }
  }
  return 0;
}

void Move_Piece_TTRIS(void)
{
  Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
  if (OU_SUIS_JE_X_ENGAGED_TTRIS == 0)
  {
    if (Check_collision_x_TTRIS(DEPLACEMENT_XX_TTRIS))
    {
      DEPLACEMENT_XX_TTRIS = 0;
    }
  }
  if (DEPLACEMENT_XX_TTRIS == 1)
  {
    xx_TTRIS++;
  }
  if (DEPLACEMENT_XX_TTRIS == -1)
  {
    xx_TTRIS--;
  }
  Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
  if (OU_SUIS_JE_X_ENGAGED_TTRIS == 0)
  {
    DEPLACEMENT_XX_TTRIS = 0;
  }

  if (Check_collision_y_TTRIS(DEPLACEMENT_YY_TTRIS))
  {
    DEPLACEMENT_YY_TTRIS = 0;
    LONG_PRESS_X_TTRIS = 0;
    Ripple_filter_TTRIS = 0;
    DROP_BREAK_TTRIS = 15;
    Tiny_Flip_TTRIS(82); // add line for refresh screen at drop
  }
  else
  {
    DROP_BREAK_TTRIS = 0;
  }
  if (DROP_SPEED_TTRIS == 0)
  {
    if (DEPLACEMENT_YY_TTRIS == -1)
    {
      yy_TTRIS--;
    }
    if (DEPLACEMENT_YY_TTRIS == 1)
    {
      yy_TTRIS++;
    }
  }
  Ou_suis_Je_TTRIS(xx_TTRIS, yy_TTRIS);
  if (OU_SUIS_JE_Y_ENGAGED_TTRIS == 0)
  {
    DEPLACEMENT_YY_TTRIS = 0;
  }
}

void Ou_suis_Je_TTRIS(int8_t xx_, int8_t yy_)
{
  int8_t xx_t, yy_t;
  xx_t = (((xx_) + 9) - 46);
  yy_t = (((yy_) + 9) - 5);
  OU_SUIS_JE_X_TTRIS = ((xx_t / 3) - 3);
  if ((xx_t % 3) != 0)
  {
    OU_SUIS_JE_X_ENGAGED_TTRIS = 1;
  }
  else
  {
    OU_SUIS_JE_X_ENGAGED_TTRIS = 0;
  }
  OU_SUIS_JE_Y_TTRIS = ((yy_t / 3) - 3);
  if ((yy_t) != ((OU_SUIS_JE_Y_TTRIS + 3) * 3))
  {
    OU_SUIS_JE_Y_ENGAGED_TTRIS = 1;
  }
  else
  {
    OU_SUIS_JE_Y_ENGAGED_TTRIS = 0;
  }
}

void Select_Piece_TTRIS(uint8_t Piece_)
{
  PIECEs_TTRIS = Piece_;
  switch (Piece_)
  {
  case 0:
    PIECEs_rot_TTRIS = 3;
    break;
  case 1:
    PIECEs_rot_TTRIS = 0;
    break;
  case 2:
  case 3:
  case 4:
    PIECEs_rot_TTRIS = 1;
    break;
  case 5:
  case 6:
    PIECEs_rot_TTRIS = 3;
    break;
  default:
    PIECEs_rot_TTRIS = 0;
    break;
  }
}

void rotate_Matrix_TTRIS(uint8_t ROT)
{
  uint8_t a_ = 0, b_ = 0;
  for (uint8_t y = 0; y < 5; y++)
  {
    for (uint8_t x = 0; x < 5; x++)
    {
      switch (ROT)
      {
      case 0:
        a_ = x, b_ = y;
        break;
      case 1:
        a_ = 4 - y, b_ = x;
        break;
      case 2:
        a_ = 4 - x, b_ = 4 - y;
        break;
      case 3:
        a_ = y, b_ = 4 - x;
        break;
      default:
        break;
      }
      Piece_Mat2_TTRIS[a_][b_] = Scan_Piece_Matrix_TTRIS(x, y + (PIECEs_TTRIS * 5));
    }
  }
}

uint8_t Scan_Piece_Matrix_TTRIS(int8_t x_Mat, int8_t y_Mat)
{
  uint8_t Result = (0b10000000 >> x_Mat) & (Pieces_TTRIS[y_Mat]);
  if (Result)
  {
    return 1;
  }
  else
  {
    return 0;
  }
  return 0;
}

uint8_t GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN)
{
  if (Y_SCAN < 0)
    return 0;
  if ((X_SCAN < 0) || (X_SCAN > 11))
  {
    return 1;
  }
  if (Y_SCAN > 18)
  {
    return 1;
  }
  uint8_t Y_VAR_SELECT = Y_SCAN >> 3;
  uint8_t Y_VAR_DECALAGE = sprite_shift_of(Y_SCAN);
  uint8_t COMP_BYTE_DECALAGE = (0b10000000 >> Y_VAR_DECALAGE);
  if ((COMP_BYTE_DECALAGE & Grid_TTRIS[X_SCAN][Y_VAR_SELECT]) == 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
  return 0;
}

uint8_t CHANGE_GRID_STAT_TTRIS(int8_t X_SCAN, int8_t Y_SCAN, uint8_t VALUE)
{
  if ((X_SCAN < 0) || (X_SCAN > 11))
    return 0;
  if ((Y_SCAN < 0) || (Y_SCAN > 18))
    return 0;
  uint8_t Y_VAR_SELECT = Y_SCAN >> 3;
  uint8_t Y_VAR_DECALAGE = sprite_shift_of(Y_SCAN);
  uint8_t COMP_BYTE_DECALAGE = (0b10000000 >> Y_VAR_DECALAGE);
  if (VALUE)
  {
    Grid_TTRIS[X_SCAN][Y_VAR_SELECT] = COMP_BYTE_DECALAGE | Grid_TTRIS[X_SCAN][Y_VAR_SELECT];
  }
  else
  {
    Grid_TTRIS[X_SCAN][Y_VAR_SELECT] = (0xff - COMP_BYTE_DECALAGE) & Grid_TTRIS[X_SCAN][Y_VAR_SELECT];
  }
  return 0;
}

uint8_t H_grid_Scan_TTRIS(uint8_t xPASS)
{
  return (H_Grid_TTTRIS[xPASS - 46]);
}

uint8_t Recupe_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  uint8_t BYTE_TTRIS = 0;
  uint8_t x = 0;
  for (uint8_t y = MEM_TTTRIS[(yPASS << 1)]; y < MEM_TTTRIS[(yPASS << 1) + 1]; y++)
  {
    if ((xPASS > 45) && (xPASS < 82))
    {
      x = H_grid_Scan_TTRIS(xPASS);
    }
    else
    {
      return (RECUPE_BACKGROUND_TTRIS(xPASS, yPASS) |
              NEXT_BLOCK_TTRIS(xPASS, yPASS) |
              recupe_Nb_of_line_TTRIS(xPASS, yPASS) |
              recupe_SCORES_TTRIS(xPASS, yPASS) |
              recupe_LEVEL_TTRIS(xPASS, yPASS));
    }
    if (GRID_STAT_TTRIS(x, y) == 1)
    {
      BYTE_TTRIS = BYTE_TTRIS | sprite_blit(46 + (x * 3), 5 + (y * 3), xPASS, yPASS, 0, tinyblock_TTTRIS);
    }
  }
  return RECUPE_BACKGROUND_TTRIS(xPASS, yPASS) | BYTE_TTRIS | DropPiece_TTRIS(xPASS, yPASS);
}

uint8_t NEXT_BLOCK_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  if (xPASS > 89)
  {
    uint8_t Byte_Mem = 0;
    uint8_t x_add = 0;
    uint8_t y_add = 0;
    switch (PIECEs_TTRIS_PREVIEW)
    {
    case 0:
      x_add = 1;
      y_add = 1;
      break;
    case 1:
      y_add = -1;
      break;
    case 2:
      x_add = 1;
      break;
    case 3:
      break;
    case 4:
      x_add = 1;
      y_add = 1;
      break;
    case 5:
      x_add = 1;
      break;
    case 6:
      break;
    default:
      break;
    }
    for (uint8_t y = 0; y < 5; y++)
    {
      for (uint8_t x = 0; x < 5; x++)
      {
        if (Scan_Piece_Matrix_TTRIS(x, y + (PIECEs_TTRIS_PREVIEW * 5)) == 1)
        {
          Byte_Mem |= sprite_blit(92 + (x * 2) + x_add, (27 + (y * 2)) - 5 + y_add, xPASS, yPASS, 0, tiny_PREVIEW_block_TTTRIS);
        }
      }
    }
    return Byte_Mem;
  }
  return 0;
}

uint8_t RECUPE_BACKGROUND_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  return (BACKGROUND_TTRIS[xPASS + (yPASS * 128)]);
}

uint8_t DropPiece_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  uint8_t Byte_Mem = 0;
  for (uint8_t y = 0; y < 5; y++)
  {
    for (uint8_t x = 0; x < 5; x++)
    {
      if (Piece_Mat2_TTRIS[x][y] == 1)
      {
        Byte_Mem |= sprite_blit(xx_TTRIS + (x * 3), (yy_TTRIS + (y * 3)) - 5, xPASS, yPASS, 0, tinyblock2_TTTRIS);
      }
    }
  }
  return Byte_Mem;
}

void Tiny_Flip_TTRIS(uint8_t HR_TTRIS)
{
  uint8_t y, x;
  Convert_Score_TTRIS();
  for (y = 0; y < 8; y++)
  {
    OLED_page_start(y);
    for (x = 0; x < HR_TTRIS; x++)
    {
      OLED_send(Recupe_TTRIS(x, y));
    }
    OLED_end();
  }
}

void Flip_intro_TTRIS(uint8_t *TIMER1)
{
  uint8_t y, x;
  Convert_Score_TTRIS();
  for (y = 0; y < 8; y++)
  {
    OLED_page_start(y);
    for (x = 0; x < 128; x++)
    {
      OLED_send(intro_TTRIS(x, y, TIMER1));
    }
    OLED_end();
  }
}

uint8_t intro_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1)
{
  return (RECUPE_BACKGROUND_TTRIS(xPASS, yPASS) |
          recupe_Chateau_TTRIS(xPASS, yPASS) |
          Recupe_Start_TTRIS(xPASS, yPASS, TIMER1) |
          recupe_SCORES_TTRIS(xPASS, yPASS) |
          recupe_Nb_of_line_TTRIS(xPASS, yPASS) |
          recupe_LEVEL_TTRIS(xPASS, yPASS));
}

uint8_t Recupe_Start_TTRIS(uint8_t xPASS, uint8_t yPASS, uint8_t *TIMER1)
{
  if (*TIMER1 > 3)
  {
    return sprite_blit(49, 28, xPASS, yPASS, 0, start_button_1_TTRIS) | sprite_blit(49, 36, xPASS, yPASS, 0, start_button_2_TTRIS);
  }
  else
  {
    return 0;
  }
}

uint8_t recupe_Chateau_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  if (xPASS < 46)
    return 0;
  if (xPASS > 81)
    return 0;
  return (chateau_TTRIS[(xPASS - 46) + (yPASS * 36)]);
}

uint8_t recupe_SCORES_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  if (xPASS < 95)
  {
    return 0;
  }
  if (xPASS > 119)
  {
    return 0;
  }
  if (yPASS > 1)
  {
    return 0;
  }
  return (sprite_blit(95, 8, xPASS, yPASS, Score_digits_TTRIS[4], police_TTRIS) |
          sprite_blit(99, 8, xPASS, yPASS, Score_digits_TTRIS[3], police_TTRIS) |
          sprite_blit(103, 8, xPASS, yPASS, Score_digits_TTRIS[2], police_TTRIS) |
          sprite_blit(107, 8, xPASS, yPASS, Score_digits_TTRIS[1], police_TTRIS) |
          sprite_blit(111, 8, xPASS, yPASS, Score_digits_TTRIS[0], police_TTRIS) |
          sprite_blit(115, 8, xPASS, yPASS, 0, police_TTRIS));
}

void Convert_Nb_of_line_TTRIS(void)
{
  Nb_of_line_TTRIS[2] = (Nb_of_line_F_TTRIS / 100);
  Nb_of_line_TTRIS[1] = ((Nb_of_line_F_TTRIS - (Nb_of_line_TTRIS[2] * 100)) / 10);
  Nb_of_line_TTRIS[0] = (Nb_of_line_F_TTRIS - (Nb_of_line_TTRIS[2] * 100) - (Nb_of_line_TTRIS[1] * 10));
}

// Score and level, least significant digit first, refreshed once per frame.
//
// The panel renderer asks for one byte at a time, so anything computed inside
// recupe_*() runs per pixel. These used to be macros expanding to a chain of
// divisions by 10000/1000/100/10, which RV32EC has to do in software - about
// thirty libcalls per pixel over the fifty pixels the score occupies.
void Convert_Score_TTRIS(void)
{
  uint16_t v = Scores_TTRIS;
  for (uint8_t i = 0; i < 5; i++)
  {
    Score_digits_TTRIS[i] = v % 10;
    v /= 10;
  }
  Level_digits_TTRIS[0] = Level_TTRIS % 10;
  Level_digits_TTRIS[1] = Level_TTRIS / 10;
}

uint8_t recupe_Nb_of_line_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  if (xPASS < 16)
  {
    return 0;
  }
  if (xPASS > 28)
  {
    return 0;
  }
  if (yPASS > 1)
  {
    return 0;
  }
  return (sprite_blit(16, 8, xPASS, yPASS, Nb_of_line_TTRIS[2], police_TTRIS) |
          sprite_blit(20, 8, xPASS, yPASS, Nb_of_line_TTRIS[1], police_TTRIS) |
          sprite_blit(24, 8, xPASS, yPASS, Nb_of_line_TTRIS[0], police_TTRIS));
}

uint8_t recupe_LEVEL_TTRIS(uint8_t xPASS, uint8_t yPASS)
{
  if (xPASS < 109)
  {
    return 0;
  }
  if (xPASS > 118)
  {
    return 0;
  }
  if (yPASS != 5)
  {
    return 0;
  }
  return (sprite_blit(109, 41, xPASS, yPASS, Level_digits_TTRIS[1], police_TTRIS) |
          sprite_blit(114, 41, xPASS, yPASS, Level_digits_TTRIS[0], police_TTRIS));
}

void INIT_ALL_VAR_TTRIS(void)
{
  for (uint8_t y = 0; y < 3; y++)
  {
    for (uint8_t x = 0; x < 12; x++)
    {
      Grid_TTRIS[x][y] = 0;
    }
  }
  for (uint8_t y = 0; y < 5; y++)
  {
    for (uint8_t x = 0; x < 5; x++)
    {
      Piece_Mat2_TTRIS[x][y] = 0;
    }
  }
  LONG_PRESS_X_TTRIS = 0;
  DOWN_DESACTIVE_TTRIS = 0;
  DROP_SPEED_TTRIS = 0;
  SPEED_x_trig_TTRIS = 0;
  DROP_TRIG_TTRIS = 1;
  xx_TTRIS = 0;
  yy_TTRIS = 0;
  Ripple_filter_TTRIS = 0;
  PIECEs_TTRIS = 0;
  PIECEs_TTRIS_PREVIEW = 0;
  PIECEs_rot_TTRIS = 0;
  DROP_BREAK_TTRIS = 0;
  OU_SUIS_JE_X_TTRIS = 0;
  OU_SUIS_JE_Y_TTRIS = 0;
  OU_SUIS_JE_X_ENGAGED_TTRIS = 0;
  OU_SUIS_JE_Y_ENGAGED_TTRIS = 0;
  DEPLACEMENT_XX_TTRIS = 0;
  DEPLACEMENT_YY_TTRIS = 0;
}

// ===================================================================================
// High score persistence
// ===================================================================================
// The ATtiny85 original kept the record in EEPROM. The CH32V003 has none, so
// the record lives in the last 64-byte page of flash, which ld/ch32v003.ld
// excludes from the image. As on the original, the title screen shows the
// record, a game over that beats it saves it, and holding DOWN when entering
// Tetris clears it.
//
// A save erases and rewrites the page (about 6 ms, only on a new record). The
// core stalls on flash access while that runs; nothing here is timing
// critical at that moment.

#define HISCORE_ADDR 0x08003FC0 // last 64-byte page, outside the image
#define HISCORE_MAGIC 0x5AA5

typedef struct
{
  uint16_t magic;
  uint16_t score;
  uint16_t lines;
  uint8_t level;
  uint8_t sum; // byte sum of the seven bytes above
} HISCORE;     // 8 bytes, no padding

static uint8_t hiscore_sum(const HISCORE *h)
{
  const uint8_t *p = (const uint8_t *)h;
  uint8_t s = 0;
  for (uint8_t i = 0; i < 7; i++)
    s += p[i];
  return s;
}

static uint8_t hiscore_valid(const HISCORE *h)
{
  return h->magic == HISCORE_MAGIC && hiscore_sum(h) == h->sum;
}

void recupe_HIGHSCORE_TTRIS(void)
{
  const HISCORE *h = (const HISCORE *)HISCORE_ADDR;
  if (hiscore_valid(h))
  {
    Level_TTRIS = h->level;
    Nb_of_line_F_TTRIS = h->lines;
    Scores_TTRIS = h->score;
  }
  else
  {
    Reset_Value_TTRIS();
  }
}

void Reset_Value_TTRIS(void)
{
  Level_TTRIS = 0;
  Nb_of_line_F_TTRIS = 0;
  Scores_TTRIS = 0;
}

void save_HIGHSCORE_TTRIS(void)
{
  union
  {
    HISCORE h;
    uint32_t w[2];
  } rec;
  rec.h.magic = HISCORE_MAGIC;
  rec.h.score = Scores_TTRIS;
  rec.h.lines = Nb_of_line_F_TTRIS;
  rec.h.level = Level_TTRIS;
  rec.h.sum = hiscore_sum(&rec.h);

  volatile uint32_t *dst = (volatile uint32_t *)HISCORE_ADDR;

  // Unlock the flash and fast-mode programming
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
  FLASH->MODEKEYR = FLASH_KEY1;
  FLASH->MODEKEYR = FLASH_KEY2;

  // Erase the 64-byte page
  FLASH->CTLR = CR_PAGE_ER;
  FLASH->ADDR = (uint32_t)dst;
  FLASH->CTLR = CR_STRT_Set | CR_PAGE_ER;
  while (FLASH->STATR & FLASH_STATR_BSY)
    ;

  // Program it: the record in the first two words, erased state after
  FLASH->CTLR = CR_PAGE_PG;
  FLASH->CTLR = CR_BUF_RST | CR_PAGE_PG;
  FLASH->ADDR = (uint32_t)dst;
  while (FLASH->STATR & FLASH_STATR_BSY)
    ;
  for (uint8_t i = 0; i < 16; i++)
  {
    dst[i] = (i < 2) ? rec.w[i] : 0xFFFFFFFF;
    FLASH->CTLR = CR_PAGE_PG | FLASH_CTLR_BUF_LOAD;
    while (FLASH->STATR & FLASH_STATR_BSY)
      ;
  }
  FLASH->CTLR = CR_PAGE_PG | CR_STRT_Set;
  while (FLASH->STATR & FLASH_STATR_BSY)
    ;

  FLASH->CTLR = CR_LOCK_Set; // lock again
}

void Check_NEW_RECORD(void)
{
  const HISCORE *h = (const HISCORE *)HISCORE_ADDR;
  if (!hiscore_valid(h) || Scores_TTRIS > h->score)
    save_HIGHSCORE_TTRIS();
}
