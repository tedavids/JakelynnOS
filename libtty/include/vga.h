//vga.h these are base VGA functions

#ifndef ARCH_I386_VGA_H
#define ARCH_I386_VGA_H

#include <stdint.h>

#include <80x25cursor.h>

// defines
#define VGA_MEMORY_START    0xFFBF8000

// enums
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
    VGA_COLOR_END 
}; // enum vga_color

// type definitions
typedef  bool (*moveCursor_fn)(bool allowscroll);

// fuctions

// get max row or column size
// Parameters:  None
// Returns:     The ow or column size

extern size_t vgaGetMaxRows();
extern size_t vgaGetMaxCols();

// Get the text color code for a character
// Parameters:  fg -- The foreground color
//              bg -- The background color

// Retuns:      the color code to use with a character
extern uint8_t vgaCreateTextColor( enum vga_color fg, enum vga_color bg);

// get the formatted text for a character on a VGA screen
// Parameters:  uc -- The character
//              color -- The foreground and background color

// Returns:     A value suitible for puting in the vga buffer
extern uint16_t vgaCreateText(unsigned char uc, uint8_t color);

// blank a row to the background color
// Parameters:  None

// Returns:     true if successful, false otherwise
extern bool vgaClearRow(size_t row);

// scroll row up one line
// Parameters:  row -- The row to scroll down
// Returns      true if successful, false otherwise
extern bool vgaScrollRowDown(size_t row);

// scroll row down one line
// Parameters:  row -- The row to scroll up
// Returns      true if successful, false otherwise
extern bool vgaScrollRowUp(size_t row);

// scroll the entire screen
// Parameters:  none
// Returns:     true if successful, false otherwise
extern bool vgaScrollAll();

// clear the entire screen to the default color
// Parameters: None
// Return:      true if successful, false otherwise;
extern bool vgaClearScreen();

// put a character on the screen
// Parameters:  color -- The color code for the character
// Returns:     true if the put is successful, false otherwise

extern bool vgaPutChar(uint8_t color, uint8_t character, struct cursor_pos_t position);

// set the text color
// Paremeters:  color -- new background color
// Returns:     old background color
extern uint8_t vgaSetTextColor(uint8_t color);

// get background color
// Parameters:  None
// Returns:     The background color
extern uint8_t vgaGetTextColor();

// enable the vga cursor
// Parameters:  type the cursor type
// Returns:     true if successful, false otherwise 
extern bool vgaEnableCursor(struct cursor_t type);

// Disable the cursor
// Parameters:  None
// Returns None
extern void vgaDisableCursor();


// initialize vga
extern bool initVGA(struct cursor_pos_t *cursor);

#endif
