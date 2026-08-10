// ===================================================================================
// Buttons, direction pad and buzzer
// ===================================================================================
// 2023 by Stefan Wagner: https://github.com/wagiminator
// 2024 by annoyedmilk: https://github.com/annoyedmilk

#include "input.h"
#include "oled_min.h"

uint8_t sound_enabled = 1;

// ===================================================================================
// Direction pad
// ===================================================================================
// The four direction buttons sit on one resistor ladder into PC4, so a single
// ADC conversion identifies which of the eight reachable positions is held.
//
// The levels below are measured, not derived from the ladder: JOY_init() runs
// the ADC at its shortest sample time, which is too short for the ladder's
// source impedance to settle, so the codes sit well below the true divider
// ratios. They are self-consistent, which is all the +/-JOY_DEV windows need,
// but they are worth re-measuring if the ADC setup ever changes.
#define JOY_DEV 20 // half-width of the window around each level

static const uint16_t PAD_LEVEL[] = {90, 197, 259, 346, 388, 511, 567, 616};
static const uint8_t PAD_MASK[] = {
    PAD_RIGHT,            // 90
    PAD_UP,               // 197
    PAD_UP | PAD_RIGHT,   // 259
    PAD_DOWN,             // 346
    PAD_DOWN | PAD_RIGHT, // 388
    PAD_LEFT,             // 511
    PAD_UP | PAD_LEFT,    // 567
    PAD_DOWN | PAD_LEFT,  // 616
};

// One conversion, decoded to a PAD_* bitmask. Reading per direction used to
// let a diagonal be seen as two different pure directions within one frame.
uint8_t JOY_pad(void)
{
  uint16_t val = (uint16_t)funAnalogRead(PIN_PAD_ANALOG);
  for (uint8_t i = 0; i < sizeof(PAD_MASK); i++)
    if ((uint16_t)(val - (PAD_LEVEL[i] - JOY_DEV)) < 2 * JOY_DEV)
      return PAD_MASK[i];
  return 0;
}

// ===================================================================================
// Buzzer
// ===================================================================================
// Square wave at 1/(2 * (255 - freq)) us, held for dur half-cycles, so larger
// freq is higher pitched and larger dur is longer.
void JOY_sound(uint8_t freq, uint8_t dur)
{
  if (!sound_enabled)
    return;
  while (dur--)
  {
    if (freq)
      PIN_low(PIN_BEEP);
    DLY_us(255 - freq);
    PIN_high(PIN_BEEP);
    DLY_us(255 - freq);
  }
}

// ===================================================================================
// Pseudo random number generator
// ===================================================================================
static uint16_t rnval = 0xACE1;

uint16_t JOY_random(void)
{
  rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
  return rnval;
}

// ===================================================================================
// Bring-up
// ===================================================================================
void JOY_init(void)
{
  SystemInit(); // HSI, no PLL, no AHB division: 24 MHz

  funGpioInitAll();

  funPinMode(PIN_PAD, GPIO_CFGLR_IN_ANALOG);
  funPinMode(PIN_ACT, GPIO_CFGLR_IN_PUPD);
  PIN_high(PIN_ACT); // input pull-up: direction is chosen by the output latch
  funPinMode(PIN_BEEP, GPIO_CFGLR_OUT_10Mhz_PP);
  PIN_high(PIN_BEEP);

  funAnalogInit();

  // Shortest sample time. The PAD_LEVEL thresholds above are calibrated
  // against it; funAnalogInit() leaves every channel on a longer one.
  ADC1->SAMPTR1 = 0;
  ADC1->SAMPTR2 = 0;

  OLED_init();
}
