// ps2kbd.c -- functions available for our keyboard handler

/* Copyright (C) 2026 Tom Davidson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _PS2KBD_H_
#define _PS2KBD_H_

#include <stdint.h>

#define KBD_EXTENDED_KEYCODE        0b0000000100000000
#define KBD_ALT_PRESSED             0b0000001000000000
#define KBD_CTRL_PRESSED            0b0000010000000000

// define keys
#define KBD_KEY_ESC                 0x1B

// init the keyboard

#ifndef _PS2KBD_C
extern bool initKbd();
extern bool kbdBufferEmpty();
extern bool kbdBufferFull();
extern uint16_t kbdGetKey();
extern uint16_t kbdPeekKey();

#endif

#endif