# Yet Another PCB Business Card 🎮

Turn your pocket into an arcade with this business-card-sized gaming console! Powered by the CH32V003 RISC-V microcontroller, this project brings classic gaming to an ultra-portable form factor.

![Business Card Front](documentation/RenderFront.png)
![Business Card Back](documentation/RenderBack.png)

## ✨ Features

- **Ultra-Compact Design**: Fits in your wallet while packing serious gaming power
- **Powerful Brain**: Built around the CH32V003 RISC-V microcontroller
- **Crystal Clear Display**: 0.96" OLED screen (SSD1315) for crisp visuals
- **Long-lasting Power**: Rechargeable LIR2032 battery
- **Intuitive Controls**: 5 tactile buttons for precise gaming input
- **Open Source**: Full hardware and software documentation available

## 🎯 Games Library

### Tiny Tris
- Classic block-falling gameplay
- Increasing difficulty levels
- Score tracking

### Tiny Pacman
- Authentic maze navigation
- Ghost AI patterns
- Power pellets and scoring system

## 🛠️ Build Your Own

### Components Needed

- [ ] PCB (Gerber files provided)
- [ ] CH32V003J4M6 microcontroller
- [ ] 0.96" OLED display (SSD1315) - X096-2864KSWAG01-H30
- [ ] 5 tactile switches (TS11092604)
- [ ] 1 slide switch (SK-3296S-01-L1)
- [ ] LIR2032 battery holder (3034TR)
- [ ] 4kHz buzzer (MLT-5030)
- [ ] Additional components (See hardware/BOM.csv for complete list)

### Assembly Instructions

1. Order PCB using provided Gerber files (`hardware/Gerber.zip`)
2. Gather components from BOM (`hardware/BOM.csv`)
3. Refer to the interactive BOM for component placement (`hardware/InteractiveBOM.html`)
4. Use pick and place file for automated assembly (`hardware/PickAndPlace.csv`)
5. Install firmware using one of the methods below

### Firmware Installation Options

#### Option 1: PlatformIO (Recommended)
1. Install required tools:
   - Install [Python 3.x](https://www.python.org/downloads/)
   - Install [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
   - Install platform-ch32v: 
     ```bash
     pio pkg install platform-ch32v
     ```
2. Clone and build:
   ```bash
   git clone https://github.com/annoyedmilk/Business-Card.git
   cd Business-Card/software
   pio run -t upload
   ```

#### Option 2: Cross-platform Tools
- **minichlink**: ([GitHub](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink))
- **rvprog**: `pip install rvprog && rvprog -f <firmware>.bin`
- **WCH-LinkUtility** (Windows): ([Download](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html))

## 🔌 Power Consumption
Comprehensive power analysis conducted using Nordic Power Profiler Kit II. The device operates efficiently across different modes:

- **Main Menu**: 21.4mA average @ 3.6V
- **Tetris**: 11.86mA average @ 3.6V  
- **Pacman**: 9.58mA average @ 3.6V
- **Buzzer**: 30-50mA average, 120mA peaks
- **Minimum operating voltage**: 2.45V (stable), 2.4V (unstable)

[See detailed power measurements](documentation/power_measurement.md)

## 📁 Project Structure

```
├── documentation/
│   ├── RenderFront.png         # PCB front view
│   ├── RenderBack.png          # PCB back view
│   ├── current_ripple.png      # Power measurement graph
│   └── power_measurement.md    # Detailed power analysis
├── hardware/
│   ├── 3D.step                 # 3D model file
│   ├── BOM.csv                 # Bill of materials
│   ├── Gerber.zip              # PCB manufacturing files
│   ├── InteractiveBOM.html     # Interactive assembly guide
│   ├── PickAndPlace.csv        # SMD placement coordinates
│   ├── ProPrj_Business-Card.epro # EasyEDA project file
│   └── Schematic.pdf           # Circuit schematic
└── software/
    ├── src/                    # Source code
    │   ├── main.c              # Main application
    │   ├── oled_min.c/h        # OLED display driver
    │   ├── i2c_tx.c/h          # I2C communication
    │   ├── system.c/h          # System initialization
    │   ├── spritebank.h        # Game sprites
    │   └── ...                 # Additional drivers
    ├── ld/ch32v003.ld          # Linker script
    ├── platformio.ini          # Build configuration
    └── COPYING.txt             # GPL license
```

## 🎮 Gaming Guide

1. Insert charged LIR2032 battery
2. Power on using the side switch
3. Navigate menu with direction buttons
4. Select game with action button
5. Controls:
   - Tiny Tris: Left/Right to move, Act to rotate, Down for soft drop
   - Tiny Pacman: Directional buttons to navigate, avoid ghosts!

## 🙏 Credits

This project builds on the work of amazing developers:

- **Stefan Wagner** ([@wagiminator](https://github.com/wagiminator)) - CH32V003 framework
- **Charles Lohr** ([@cnlohr](https://github.com/cnlohr)) - CH32V003 tools & community support
- **Daniel C** ([TinyJoypad](https://www.tinyjoypad.com)) - Original game implementations

## 📄 License

Released under GPL-3.0 License. Share, modify, and spread the joy of tiny gaming!

## 🔗 Useful Links

- [PlatformIO Installation](https://platformio.org/)
- [CH32V Platform Guide](https://pio-ch32v.readthedocs.io/en/latest/installation.html)
- [Python Installation Guide](https://www.pythontutorial.net/getting-started/install-python/)
- [pyenv Setup](https://realpython.com/intro-to-pyenv/)

---

*Making the world a little more fun, one tiny arcade at a time! 🕹️*
