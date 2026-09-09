// process the kernel command line

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

#ifndef _KRNLCMDLINE_H
#define _KRNLCMDLINE_H

#include <stdint.h>
#include <stdbool.h>

struct cmdline_options_t {
    uint32_t    heapSizePages;
    bool        useVideo;
};

extern struct cmdline_options_t cmdLineOpts;

// process the kernel command line and set the command line options structure

// Parameters:  cmdline -- The command line

// returns:     true if all OK, false otherwise

extern bool processCommandLine(const char * const __restrict cmdline);


#endif