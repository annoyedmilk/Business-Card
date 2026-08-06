# Business Card

A business-card-sized handheld games console built around the CH32V003
RISC-V microcontroller: a 0.96" I2C OLED, five buttons, a piezo buzzer
and a rechargeable LIR2032 coin cell. It runs Tiny Tris, Tiny Pacman and
a hardware test mode, ported from the TinyJoypad project.

![Front](documentation/RenderFront.png)
![Back](documentation/RenderBack.png)
![Assembled board](documentation/Business-Card.jpg)

Hardware and firmware are released under GPL-3.0. See `LICENSE`.

## Hardware

| Function     | Part                    | Notes                                         |
| ------------ | ----------------------- | --------------------------------------------- |
| MCU          | CH32V003J4M6            | RV32EC, 16K flash, 2K SRAM, SOP-8             |
| Display      | X096-2864KSWAG01-H30    | 0.96" 128x64 OLED, SSD1315, I2C               |
| Buttons      | 5x TS11092604           | 4 directions on a resistor ladder, 1 action   |
| Audio        | MLT-5030                | Piezo buzzer                                  |
| Power        | 3034TR holder + LIR2032 | Rechargeable coin cell, SK-3296S slide switch |

The SOP-8 package leaves few pins: PA1 buzzer, PA2 action button, PC1/PC2
OLED I2C, PC4 direction buttons (ADC ladder).

`hardware/` contains the EasyEDA project, schematic PDF, Gerbers, BOM,
pick-and-place data, an interactive BOM and a STEP model. To build a
board, send `Gerber.zip` to a fabricator and order the parts in
`BOM.csv`.

## Firmware

Bare-metal C on [ch32fun](https://github.com/cnlohr/ch32fun), vendored
as a submodule. No vendor SDK.

    software/
      Makefile            Build configuration
      funconfig.h         ch32fun configuration (clock, SysTick)
      ld/ch32v003.ld      Linker script; reserves the high-score flash page
      ch32fun/            Submodule
      src/
        main.c            Menu, Tiny Tris, Tiny Pacman, test mode
        driver.h          Joypad, display and frame pacing layer
        i2c_tx.c/.h       Transmit-only I2C master
        oled_min.c/.h     Minimal SSD1315 driver
        spritebank.h      Sprite and bitmap data

The core runs at 12 MHz and the I2C bus at 1 MHz. Game speed does not
depend on either: `frame_sync()` in `driver.h` holds each pass of a game
loop to a fixed period, since the ATtiny originals carry no frame timing
of their own. Pixel data reaches the panel by DMA so the core can sleep
through the transfer.

Rationale for the clocking, pacing, sleeping and data compression is
commented where each lives: `funconfig.h`, `driver.h`, `i2c_tx.h` and
`spritebank.h`.

### Build

    git submodule update --init
    cd software
    make

Needs GNU Make and a RISC-V GCC on `PATH`; the
[xPack riscv-none-elf-gcc](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases)
works without an installer (on Windows, unpack it to a short path).
Output: `software/src/main.bin`.

### Flash

    make flash

drives a WCH-LinkE via the bundled minichlink. On Windows, minichlink
needs the WinUSB driver on WCH-Link interface 0 (installable with
[Zadig](https://zadig.akeo.ie/)); alternatively flash `main.bin` with
WCH-LinkUtility using the stock driver.

## Usage

Fit a charged LIR2032 and switch on. Directions move through the menu,
the action button selects: Tiny Tris, Pacman, test mode, sound toggle.

- Tiny Tris: left/right move, action rotates, down soft-drops.
- Pacman: directions steer.
- Test mode: for board bring-up. A marker follows the direction pad and
  springs back to centre on release, an indicator lights while action is
  held, and a tone sweep on entry checks the buzzer.

Holding the action button for about a second returns to the menu from
any of the three. The panel blanks after 30 seconds idle in the menu and
wakes on any button.

The Tiny Tris high score survives power-off: it is kept in the last
64-byte page of flash, which the linker script keeps out of the image.
The title screen shows the record; beating it at game over saves it.
Holding down while Tetris starts clears it.

## Credits

- Daniel C ([TinyJoypad](https://www.tinyjoypad.com)) - original Tiny
  Tris, Tiny Pacman and TESTMODE for the ATtiny85.
- Stefan Wagner ([@wagiminator](https://github.com/wagiminator)) - CH32V003
  port of Tiny Tris, I2C and OLED drivers.
- Charles Lohr ([@cnlohr](https://github.com/cnlohr)) - ch32fun and
  minichlink.
- Marco Müller - board design and CH32V003 game integration.
