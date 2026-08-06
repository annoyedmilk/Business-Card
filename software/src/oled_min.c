// ===================================================================================
// SSD1306/SSD1315 128x64 Pixels OLED Minimal Functions                       * v1.0 *
// ===================================================================================
// 2022 by Stefan Wagner: https://github.com/wagiminator

#include "oled_min.h"

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
    OLED_MULTIPLEX, 0x3F,            // set multiplex ratio
    OLED_CONTRAST, OLED_BRIGHTNESS,  // set panel drive level
    OLED_CHARGEPUMP, 0x14,           // set DC-DC enable
    OLED_MEMORYMODE, 0x00,    // set horizontal addressing mode
    OLED_COLUMNS, 0x00, 0x7F, // set start and end column
    OLED_PAGES, 0x00, 0x3F,   // set start and end page
    OLED_COMPINS, 0x12,       // set com pins
    OLED_XFLIP, OLED_YFLIP,   // flip screen
    OLED_DISPLAY_ON           // display on
};

// OLED init function
void OLED_init(void)
{
  uint8_t i;
  I2C_init();               // initialize I2C first
  I2C_start(OLED_ADDR);     // start transmission to OLED
  I2C_write(OLED_CMD_MODE); // set command mode
  for (i = 0; i < sizeof(OLED_INIT_CMD); i++)
    I2C_write(OLED_INIT_CMD[i]); // send the command bytes
  I2C_stop();                    // stop transmission
}

// Send a single command byte
void OLED_send_command(uint8_t cmd)
{
  I2C_start(OLED_ADDR);
  I2C_write(OLED_CMD_MODE);
  I2C_write(cmd);
  I2C_stop();
}

// OLED set cursor position
void OLED_setpos(uint8_t x, uint8_t y)
{
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_CMD_MODE);               // set command mode
  I2C_write(OLED_PAGE | y);               // set page start address
  I2C_write(x & 0x0F);                    // set lower nibble of start column
  I2C_write(OLED_COLUMN_HIGH | (x >> 4)); // set higher nibble of start column
  I2C_stop();                             // stop transmission
}
