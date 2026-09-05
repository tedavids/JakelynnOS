// define your cursor structure (80x25) 
// Note:  cursor is 0 based for row and columns

#ifndef _80x25CURSOR_H
#define _80x25CURSOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define     CURSOR_BLOCK            1
#define     CURSOR_UNDERLINE        2
#define     CURSOR_THICK_UNDERLINE  3

#define     CURSOR_80X25_ERROR      65535

struct cursor_pos_t {
    size_t row;
    size_t col;
};

struct cursor_t {
    uint8_t firstscanline;
    uint8_t lastscanline;
};

extern struct cursor_t VGA_CURSOR_BLOCK;
extern struct cursor_t VGA_CURSOR_UNDERLINE;
extern struct cursor_t VGA_CURSOR_THICK_UNDERLINE;

extern struct cursor_t cursor_block;
extern struct cursor_t cursor_underline;
extern struct cursor_t cursor_thick_underline;

// set the active cursor
// Parameters:  cursor -- A pointer to a cursor_pos_t object
// Returns:     The pointer to the current active cursor
extern struct cursor_pos_t* cursorSetActiveCursor(struct cursor_pos_t * cursor);

// get the active cursor
// Parameters:  None
// Returns:     a pointer to the current active cursor
extern struct cursor_pos_t *cursorGetActiveCursor();

// get maximum lineal address of cursor
// Parameters: None
// Returns:     the maximum lineal address
extern size_t cursorGetMax80x25LinealOffset();

// get max rows and columns for the cursor
// Parameters:  None
// Returns:     The maximum value for either row or column
extern size_t cursorGetMax80x25Cols();
extern size_t cursorGetMax80x25Rows();

// set the cursor position for VGA 80x25 text mode

// inputs:  row -- the row the cursor should be put on
//          col -- the col the cursor should be put on

// returns: true -- if row and column are valid
//          false -- if either is out of bounds
extern bool cursorSet80x25CursorPosXY(size_t row, size_t col);

// set cursor to beginning of current line
// Parameters:  None
// returns:     true if successful
extern bool cursorSet80x25CursorToBeginOfLine();

// move cursor

// move cursor up one row
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise

extern bool cursorMove80x25CursorUp(bool allowscroll);

// move cursor down one row
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise
extern bool cursorMove80x25CursorDown(bool allowscroll);

// move cursor one position to the left
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise
extern bool cursorMove80x25CursorLeft(bool allowscroll);

// move the cursor one position to the right
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise
extern bool cursorMove80x25CursorRight(bool allowscroll);

// validate the cursor

// inputs:  cursor  a pointer to the cursor to validate

// returns: bool    true if the cursor is valid, false if it isn't

// note: use set80x25CursorPosXY to fix if it returns false;

extern bool cursorValidate80x25Cursor(struct cursor_pos_t cursorposition);

// get a pointer to the current cursor
// Parameters:  None
// returns      a pointer to the current cursor
extern struct cursor_t* cursorGetCurrentCursor();

// get the current cursor
// Parameters:  None
// Returns:     the current cursor type

extern int cursorGetCurrentCursorType();

// set the current cursor
// Parameters:  cursor -- new current cursor type
// Returns:     true if successful, false otherwise

bool cursorSetCursorType(int type);

// get cursor scan lines

// parameters:  None

// returns:     The scan line
extern uint8_t cursorGet80x25CursorFirstScanLine();
extern uint8_t cursorGet80x25CursorLastScanLine();

// get the position of the cursor in the VGA buffer
// Parameters:  cursorpos -- The cursor position

// Returns:     The position in the buffer
extern uint16_t cursorCursorPos80x25ToLineal(struct cursor_pos_t cursorpos);

// get the cursor position given the position in the VGA buffer
// Parameter:   lineal -- The buffer position

// Returns:     The curpos in cursor_pos_t format given a lineal address
extern struct cursor_pos_t cursorLineal80x25ToCursorpos(uint16_t lineal);

// initialize the cursor
// Parameters:  rows -- number of rows on screen
//              columns -- number of columns on screen
//              lastscanline -- number of scan lines in the cursor

extern bool initCursor(struct cursor_pos_t *cursor, size_t rows, size_t columns, uint8_t lastscanline);
#endif