#include "sprite.h"

uint8_t sprite_blit(int8_t xPos, int8_t yPos, uint8_t xPASS, uint8_t yPASS, uint8_t FRAME, const uint8_t *SPRITES)
{
  uint8_t OUTBYTE;
  uint8_t WSPRITE = ((SPRITES[0]));
  uint8_t HSPRITE = ((SPRITES[1]));
  uint8_t Wmax = ((HSPRITE * WSPRITE) + 1);
  uint16_t PICBYTE = FRAME * (Wmax - 1);
  uint8_t RECUPELINEY = sprite_page_of(yPos);
  if ((xPASS > ((xPos + (WSPRITE - 1)))) || (xPASS < xPos) || ((RECUPELINEY > yPASS) || ((RECUPELINEY + (HSPRITE)) < yPASS)))
  {
    return 0x00;
  }
  uint8_t SPRITEyLINE = (yPASS - (RECUPELINEY));
  uint8_t SPRITEyDECALAGE = (sprite_shift_of(yPos));
  uint8_t ScanA = (((xPASS - xPos) + (SPRITEyLINE * WSPRITE)) + 2);
  uint8_t ScanB = (((xPASS - xPos) + ((SPRITEyLINE - 1) * WSPRITE)) + 2);
  if (ScanA > Wmax)
  {
    OUTBYTE = 0x00;
  }
  else
  {
    OUTBYTE = sprite_split(SPRITEyDECALAGE, (SPRITES[ScanA + (PICBYTE)]), 1);
  }
  if ((SPRITEyLINE > 0))
  {
    uint8_t OUTBYTE2 = sprite_split(SPRITEyDECALAGE, (SPRITES[ScanB + (PICBYTE)]), 0);
    if (ScanB > Wmax)
    {
      return OUTBYTE;
    }
    else
    {
      return OUTBYTE | OUTBYTE2;
    }
  }
  else
  {
    return OUTBYTE;
  }
}
