// ===================================================================================
// Project:   Tiny Tris and Tiny Pacman - Conversion for CH32V003
// Version:   v1.2
// ===================================================================================
//
//         >>>>>  T-I-N-Y  T-R-I-S v3 for ATTINY85  GPL v3 <<<<<
//                     Programmer: Daniel C 2019-2021
//              Contact EMAIL: electro_l.i.b@tinyjoypad.com
//                      https://WWW.TINYJOYPAD.COM
//           https://sites.google.com/view/arduino-collection

//          >>>>>  T-I-N-Y  P-A-C-M-A-N for ATTINY85  GPL v3 <<<<
//                   Programmer: Daniel C 2018-2021
//             Contact EMAIL: electro_l.i.b@tinyjoypad.com
//           https://sites.google.com/view/arduino-collection

//  Tiny-tris and Tiny-pacman are free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ===================================================================================
// Modified to work with CH32V003 by Stefan Wagner: https://github.com/wagiminator
// Modified to include both Tiny Tris and Tiny Pacman by annoyedmilk: https://github.com/annoyedmilk
// ===================================================================================

#include "input.h"
#include "pacman.h"
#include "testmode.h"
#include "tetris.h"
#include "ui.h"

int main(void)
{
  JOY_init();

  uint8_t state = MENU_STATE;

  // Show startup logo; handle_menu() paints the menu over it on its first
  // pass through the main loop.
  show_startup_logo();
  DLY_ms(1000);
  play_start_jingle();

  while (1)
  {
    switch (state)
    {
    case MENU_STATE:
      handle_menu(&state);
      break;

    case TETRIS_STATE:
      handle_tetris_game(&state);
      break;

    case PACMAN_STATE:
      handle_pacman_game(&state);
      break;

    case TEST_STATE:
      handle_test_mode(&state);
      break;
    }
  }
}
