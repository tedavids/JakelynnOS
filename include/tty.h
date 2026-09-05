// tty.h  functions provided by the libtty.a object

#ifndef __KERNEL_TTY_H
#define __KERNEL_TTY_H

#include <stddef.h>

#include <vga.h>

// initialize the screen

// Parameters:  None

// Returns:     true on success, false otherwise
extern bool ttyInit(void);

// put a character to the scree at the current cursor positon

// Parameters:  c -- The character to print

// Returns:     None
extern bool ttyPutChar(char c);

// Write data to the screen

// Parameters:  data -- The data to display
//              size -- The length of the data

// Returns:     None
extern bool ttyWrite(const char* data, size_t size);

// Write a string to the screen

// Parameters:  data -- The string to display

// Returns:     None
extern bool ttyWriteString(const char* data);

// set the color of the terminal characters
// Parameters:  color -- the new color
// returns:     the old color
extern uint8_t ttySetColor(uint8_t color) ;

// scroll the entire screen
// Parameters:  None

// Return:      None
extern bool ttyScrollScreen();

// clear the last line of the screen

// Parameters:  None

// Returns:     true if successful, false otherwise
extern bool ttyClearLastLine();

// Scroll a single line up 1 line

// Parameters:  line -- The line to scroll

// Returns:     None
extern bool ttyScrollRowUp(size_t line);

// Scroll a single line down 1 line

// Parameters:  line -- The line to scroll

// Returns:     None
extern bool ttyScrollRowDown(size_t line);

// scroll up beginning at line row
// Parameters:  row -- The first row to scroll up (it will disappar)
// returns:     True if successful, false otherwise
extern bool ttyScrollUpFrom(size_t row);

// clear a line on the screen

// Parameters:  line -- The line to clear

// Returns:     true if the line is valid
extern bool ttyClearRow(size_t row);

// clear the screen

// Parameters:  None

// Returns:     true if successful, false otherwise
extern bool ttyClearScreen();

// clear a ranger of rows

// Parameters:  first -- The first row to clear
//              last -- The last row to clear
extern bool ttyClearRows(size_t first, size_t last);

#endif
