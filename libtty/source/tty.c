// tty.c  These are our initial screen functions used at boot time

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vga.h>

#include <tty.h>

// typedefs
typedef bool (*setCursorType_fn)(int);
typedef bool (*validateCursor_fn)(struct cursor_pos_t ttyCursorPosition);
typedef bool (*setCursorAt_fn)(size_t row, size_t col);
typedef bool (*setCursorToBeginOfLine_fn)();

moveCursor_fn   ttyCursorUp;
moveCursor_fn   ttyCursorDown;
moveCursor_fn   ttyCursorLeft;
moveCursor_fn   ttyCursorRight;

setCursorType_fn setCursorType;

validateCursor_fn ttyValidateCursor;

setCursorAt_fn  setCursorAt;
setCursorToBeginOfLine_fn setCursorToBeginOfLine;

struct cursor_pos_t ttyCursorPosition;
uint8_t characterColor;

bool ttyInit(void) 
{
    if (!initVGA(&ttyCursorPosition)) return false;

    // usign these for future different resolutions

    ttyCursorUp = cursorMove80x25CursorUp;
    ttyCursorDown = cursorMove80x25CursorDown;
    ttyCursorLeft = cursorMove80x25CursorLeft;
    ttyCursorRight = cursorMove80x25CursorRight;

    setCursorType = cursorSetCursorType;
    ttyValidateCursor = cursorValidate80x25Cursor;
    setCursorAt = cursorSet80x25CursorPosXY;
    setCursorToBeginOfLine = cursorSet80x25CursorToBeginOfLine;

    // returns old color, we just discard it
    characterColor = vgaSetTextColor(vgaCreateTextColor(VGA_COLOR_LIGHT_GREY,VGA_COLOR_BLACK));

    bool rtncde = vgaClearScreen();

    if (rtncde) {
        // set a block cursor
        rtncde &= cursorSetCursorType(CURSOR_BLOCK);
        if (rtncde) {
            rtncde &= setCursorAt(0,0);
        }
    }
    return rtncde;
} // void terminal_initialize(void)

// clear a terminal row
// Paramaters:  row -- the row to clear
// Returns:     true if successful, false otherwise
bool ttyClearRow(size_t row) {
    return vgaClearRow(row);
}

bool ttyClearRows(size_t first, size_t last) {
    // make sure we are in range
    if (first >= vgaGetMaxRows()) return false;
    if (last >= vgaGetMaxRows()) return false;
    if (first > last) return false;

    bool rtncde = true;
    // clear the rows
    for (size_t row = first; row <= last; row++) {
        rtncde &= vgaClearRow(row);
    }
     
    return rtncde;
}

bool ttyClearScreen() {
  
    return vgaClearScreen();
}

uint8_t ttySetColor(uint8_t color) 
{
    return vgaSetTextColor(color);
}

bool ttyPutEntryAt(unsigned char c, uint8_t color, struct cursor_pos_t cursorpos)
{
    // validate parameters
    if (!ttyValidateCursor(cursorpos)) return false;

    // write the character
    return vgaPutChar(color,c,cursorpos);
} 

bool ttyScrollRowUp(size_t row) 
{ 
    return vgaScrollRowUp(row);
} 

// Scroll a single line down 1 line

// Parameters:  line -- The line to scroll

// Returns:     None
bool ttyScrollRowDown(size_t line) {
    return vgaScrollRowDown(line);
}

bool ttyClearLastLine() {
    return vgaClearRow(vgaGetMaxRows() - 1);
    
} 

// scroll up beginning at line row
// Parameters:  row -- The first row to scroll up (it will disappar)
// returns:     True if successful, false otherwise
bool ttyScrollUpFrom(size_t row) {
    // check parameters
    if (row >(vgaGetMaxRows() - 1)) return false;

    // last row just blank it
    if (row == (vgaGetMaxRows() - 1)) return ttyClearLastLine();
    
    // if first row scroll entire screen
    if (row == 0) return vgaScrollAll();

    // we need to actually do something

    bool rtncde = true;

    for (size_t i = row; i < vgaGetMaxRows(); i++) {
        rtncde &= vgaScrollRowUp(i);
    }

    return rtncde;
}

// scroll the whole window
bool ttyScrollScreen() {
    return ttyScrollUpFrom(1);
} // void terminal_scroll()

bool ttyPutChar(char c) 
{
    unsigned char uc = (unsigned char) c;

    // so we can handle non-printing characters
    switch (uc) {
    case '\n':
        if (ttyCursorDown(true)) {
            return ttyScrollUpFrom(0);
        }
        return true;
    case '\r':
        return setCursorToBeginOfLine();
    default:
        if (!ttyPutEntryAt(uc,vgaGetTextColor(),ttyCursorPosition)) return false;
        // move cursor and scroll if necessary
        if ( ttyCursorRight(true)) {
            if (ttyCursorDown(true)) {
                return ttyScrollUpFrom(0);
            }
        }
        return true;
    } // switch (c)
 

}


bool ttyWrite(const char* data, size_t size) 
{
    for (size_t i = 0; i < size; i++) {
        if (!ttyPutChar(data[i])) return false;
    } // for (size_t i = 0; i < size; i++)

    return true;
} 

bool ttyWriteString(const char* data) 
{
    return ttyWrite(data, strlen(data));
}
