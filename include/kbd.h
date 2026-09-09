// keyboard functions

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

#ifndef _KBD_H
#define _KBD_H

// get keyboard input
// Parameters:  buffer -- the buffer for the input
//              bufflen -- length of the buffer
//              echo -- do we echo it to the screen
// Returns:     the number of characters retuned
extern int kbdInput(char* restrict buffer, int bufflen, bool echo);

#endif