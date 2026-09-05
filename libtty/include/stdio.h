// our standard io (stdio.h) implementation
#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>
#include "sys/cdefs.h"
#include <tty.h>

#define EOF (-1)


// print functions
// Print a formatted string to the screen, at the current cursor positon

//  Parameters: string -- the format string (see snprintf for values)
//              ... -- a list of values to fill the format parameters

//  Returns:    The number of characters printerd

extern int printf(const char* string, ...);

// print unformatted string
// Parameters:  string -- String to print
// Returns:     true if successful, false otherwise
extern bool print(const char* string);

// Put an individual character to the screen, at the current cursor position

// Parameter:   ic -- the character to print

// Returns:     The character printed

extern int putchar(int ic);

// Print a string at the current cursor position

//  Parameter:  string -- The string to print

//  Returns:    The number of characters printed

extern int puts(const char* string);


#endif