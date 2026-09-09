// docommand from the shell

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

#ifndef _DOCMD_H
#define _DOCMD_H

#include <stdint.h>

// Process a particular command internal or external

// Parameters:  command - The command to execute

// Returns:     integer - 0 if it finishes correctly
//                        value - the return code if it doesn't
//                        value should be < 0 if failure, > 0 if completed with warnings

extern int doCommand(const char* command);

#endif