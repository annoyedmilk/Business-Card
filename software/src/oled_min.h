// ===================================================================================
// SSD1306/SSD1315 128x64 Pixels OLED Minimal Functions                       * v1.0 *
// ===================================================================================
//
// 2022 by Stefan Wagner: https://github.com/wagiminator
//
// ch32fun ships an SSD1306 driver of its own, but it renders out of a
// 1024-byte framebuffer, which does not fit alongside the games in 2K of RAM.
// This one streams straight to the panel a page at a time instead.

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "i2c_tx.h"

// OLED definitions
#define OLED_ADDR 0x78     // OLED write address (0x3C << 1)
#define OLED_CMD_MODE 0x00 // set command mode
#define OLED_DAT_MODE 0x40 // set data mode

// Panel drive level, 0x00 to 0xFF. The OLED draws far more than the MCU, and
// its current tracks this almost linearly, so it is the cheapest thing to
// trade for battery life. 0x7F is the controller default, which is brighter
// than this display needs indoors.
#define OLED_BRIGHTNESS 0x40

// OLED commands used by the init sequence and the cursor
#define OLED_COLUMN_HIGH 0x10 // set higher 4 bits of start column (0x10 - 0x1F)
#define OLED_CONTRAST 0x81    // set panel drive level (following byte)
#define OLED_MEMORYMODE 0x20  // set memory addressing mode (following byte)
#define OLED_COLUMNS 0x21     // set start and end column (following 2 bytes)
#define OLED_PAGES 0x22       // set start and end page (following 2 bytes)
#define OLED_CHARGEPUMP 0x8D  // (following byte - 0x14:enable, 0x10: disable)
#define OLED_XFLIP 0xA1       // flip display horizontally
#define OLED_MULTIPLEX 0xA8   // set multiplex ratio (following byte)
#define OLED_DISPLAY_OFF 0xAE // set display off (panel sleeps)
#define OLED_DISPLAY_ON 0xAF  // set display on
#define OLED_PAGE 0xB0        // set start page (following byte)
#define OLED_YFLIP 0xC8       // flip display vertically
#define OLED_COMPINS 0xDA     // set COM pin config (following byte)

    // Functions
    void OLED_init(void);
    void OLED_setpos(uint8_t x, uint8_t y);
    void OLED_send_command(uint8_t cmd);

#ifdef __cplusplus
};
#endif
