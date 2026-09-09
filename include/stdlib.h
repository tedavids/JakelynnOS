// our standard lib

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

#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// abort everything
__attribute__((__noreturn__))

// abort the kernel

extern void abort(void);

// "print" to a string

// return a formatted string 

// Parameters:  buff -- the output buffer
//              buffsize -- The length of the output buffer
//              format -- The format string
//              ... -- A variable list of items to fill the format 

// returns:     The lenght of the string
extern int snprintf(char* restrict buff, size_t buffsize, const char* restrict format, ...);

// return a formatted string 

// Parameters:  buff -- the output buffer
//              buffsize -- The length of the output buffer
//              format -- The format string
//              ... -- A variable list of items to fill the format 

// returns:     The lenght of the string
extern int snprintfint(char* restrict buff, size_t buffsize, const char* restrict format, va_list* parameters);

#ifdef __cplusplus
}
#endif

#endif