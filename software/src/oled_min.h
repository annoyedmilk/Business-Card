// ===================================================================================
// SSD1306/SSD1315 128x64 Pixels OLED Minimal Functions                       * v1.0 *
// ===================================================================================
//
// 2022 by Stefan Wagner: https://github.com/wagiminator
//
// ch32fun ships two SSD1306 drivers, neither of which fits here:
//
// - extralibs/ssd1306.h renders out of a 1024-byte framebuffer, which does not
//   fit alongside the games in 2K of RAM.
// - extralibs/ssd1306_i2c.h is packet-based, and ssd1306_pkt_send() builds its
//   packet in a 33-byte stack buffer, so it cannot carry a 128-byte page. Its
//   error path also calls printf, which funconfig.h disables to save flash.
//
// This one streams straight to the panel a page at a time instead. Its bus
// setup still follows ssd1306_i2c.h; see i2c_tx.c.

#pragma once

#include "i2c_tx.h"

// OLED definitions
#define OLED_ADDR 0x78     // OLED write address (0x3C << 1)
#define OLED_CMD_MODE 0x00 // set command mode
#define OLED_DAT_MODE 0x40 // set data mode

// OLED commands used by the init sequence and the cursor
#define OLED_COLUMN_HIGH 0x10 // set higher 4 bits of start column (0x10 - 0x1F)
#define OLED_MEMORYMODE 0x20  // set memory addressing mode (following byte)
#define OLED_COLUMNS 0x21     // set start and end column (following 2 bytes)
#define OLED_PAGES 0x22       // set start and end page (following 2 bytes)
#define OLED_CHARGEPUMP 0x8D  // (following byte - 0x14:enable, 0x10: disable)
#define OLED_XFLIP 0xA1       // flip display horizontally
#define OLED_MULTIPLEX 0xA8   // set multiplex ratio (following byte)
#define OLED_DISPLAY_ON 0xAF  // set display on
#define OLED_PAGE 0xB0        // set start page (following byte)
#define OLED_YFLIP 0xC8       // flip display vertically
#define OLED_COMPINS 0xDA     // set COM pin config (following byte)

// Functions
void OLED_init(void);
void OLED_data_start(void);
void OLED_setpos(uint8_t x, uint8_t y);

// Page streaming. The renderers compute one byte per column and hand it
// straight to the bus, so no framebuffer is needed.
#define OLED_page_start(y) \
  {                        \
    OLED_setpos(0, y);     \
    OLED_data_start();     \
  }
#define OLED_send(b) I2C_write(b)
#define OLED_end I2C_stop
