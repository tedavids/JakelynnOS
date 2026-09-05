// your cursor functions

#include <string.h>

#include "80x25cursor.h"

#define CURSOR_80X25_MAX_LINEAL_ADDRESS ((max80x25cols) * (max80x25rows))


struct cursor_pos_t *cursorPosition = nullptr;

struct cursor_t cursor_block;
struct cursor_t cursor_underline;
struct cursor_t cursor_thick_underline;

struct cursor_t current_cursor;

static size_t max80x25cols;
static size_t max80x25rows;

// set the active cursor
// Parameters:  cursor -- A pointer to a cursor_pos_t object
// Returns:     The pointer to the current active cursor
struct cursor_pos_t* cursorSetActiveCursor(struct cursor_pos_t * cursor) {
    auto oldcursor = cursorPosition;

    cursorPosition = cursor;

    return oldcursor;
}

// get the active cursor
// Parameters:  None
// Returns:     a pointer to the current active cursor
struct cursor_pos_t *cursorGetActiveCursor() {
    return cursorPosition;
}

// get maximum lineal address of cursor
// Parameters: None
// Returns:     the maximum lineal address
size_t cursorGetMax80x25LinealOffset() {
    return CURSOR_80X25_MAX_LINEAL_ADDRESS;
}

// get max rows and columns for the cursor
// Parameters:  None
// Returns:     The maximum value for either row or column
size_t cursorGetMax80x25Cols() {return max80x25cols;}
size_t cursorGetMax80x25Rows() {return max80x25rows;}

// set the cursor position for VGA 80x25 text mode

// inputs:  row -- the row the cursor should be put on
//          col -- the col the cursor should be put on

// returns: true -- if row and column are valid
//          false -- if either is out of bounds
bool cursorSet80x25CursorPosXY(size_t row, size_t col) {
    // check parameters
    if (!cursorPosition) return false;
    if (!(row < max80x25rows)) return false;
    if (!(col < max80x25cols)) return false;

    cursorPosition->col = col;
    cursorPosition->row = row;
    return true;
}

// set cursor to beginning of current line
// Parameters:  None
// returns:     true if successful
bool cursorSet80x25CursorToBeginOfLine() {
    // validate cursor
    if (!cursorPosition) return false;
    if (!cursorValidate80x25Cursor(*cursorPosition)) return false;

    // set it 
    cursorPosition->col = 0;

    return true;
}

// move cursor up one row
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise

bool cursorMove80x25CursorUp(bool allowscroll) {
    // make sure cursor is valid to begin with
    if (!cursorPosition) return false;
    if (!cursorValidate80x25Cursor(*cursorPosition)) return false;

    // we are at top of screen, we just have to send back if we scroll
    if (cursorPosition->row == 0) {
        return allowscroll;
    }

    //move cursor up, we know it isn't row 0
    cursorPosition->row--;

    // no scroll
    return false;
}

// move cursor down one row
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise
bool cursorMove80x25CursorDown(bool allowscroll) {
    // make sure cursor is valid to begin with
    if (!cursorPosition) return false;
    if (!cursorValidate80x25Cursor(*cursorPosition)) return false;
 
    // increment row
    cursorPosition->row++;
    if (cursorPosition->row == max80x25rows) {
        // set back to last row
        cursorPosition->row--;
        // now return if we need to scroll
        return allowscroll;
    }
    // no scroll check needed
    return false;
}

// move cursor one position to the left
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise
bool cursorMove80x25CursorLeft(bool allowscroll) {
    // make sure cursor is valid to begin with
    if (!cursorPosition) return false;
    if (!cursorValidate80x25Cursor(*cursorPosition)) return false;

    // check if we are at beginning of line
    if (cursorPosition->col == 0) {
        cursorPosition->col = max80x25cols - 1;
        // we need to scroll up
        return cursorMove80x25CursorUp(allowscroll);
    }
    cursorPosition->col--;

    // we don't need to scroll
    return false;
}

// move the cursor one position to the right
// inputs:  alllowscroll    allow a scroll

// returns: bool            true if we need to scroll, false otherwise
bool cursorMove80x25CursorRight(bool allowscroll) {
    // make sure cursor is valid to begin with
    if (!cursorPosition) return false;
    if (!cursorValidate80x25Cursor(*cursorPosition)) return false;

    cursorPosition->col++;
    // check if we are past end of line
    if (cursorPosition->col < max80x25cols) {
        // we don't need to check scroll;
        return false;
    }

    // set cursor to column 0
    cursorPosition->col = 0;
    // send back if we need to scroll
    return cursorMove80x25CursorDown(allowscroll);
}

// validate the cursor

// inputs:  cursor  a pointer to the cursor to validate
//          fix     do we fix the cursor 

// returns: bool    true if the cursor is valid (may be valid becaue its been fixed)
//                  false if the cursor is not valid and cant be fixed

extern bool cursorValidate80x25Cursor(struct cursor_pos_t cursorposition) {
    // check row
    if (!(cursorposition.row < max80x25rows)) return false;
    if (!(cursorposition.col < max80x25cols)) return false;

    // cursor is valid
    return true;
}

// get a pointer to the current cursor
// Parameters:  None
// returns      a pointer to the current cursor
extern struct cursor_t* cursorGetCurrentCursor() {
    return &current_cursor;
}

// get the current cursor
// Parameters:  None
// Returns:     the current cursor type, or zero if an error occurs

int cursorGetCurrentCursorType() {
    if (!memcmp(&current_cursor,&cursor_block,sizeof(struct cursor_t))) return CURSOR_BLOCK;
    if (!memcmp(&current_cursor,&cursor_underline,sizeof(struct cursor_t))) return CURSOR_UNDERLINE;
    if (!memcmp(&current_cursor,&cursor_thick_underline,sizeof(struct cursor_t))) return CURSOR_THICK_UNDERLINE;
    return 0;
}
// set the current cursor type
// Parameters:  type -- the new type of the cursor
// Returns:     true if successful, false otherwise

bool cursorSetCursorType(int type) {
    switch (type) {
        case CURSOR_BLOCK:
            current_cursor = cursor_block;
            return true;
        case CURSOR_UNDERLINE:
            current_cursor = cursor_underline;
            return true;
        case CURSOR_THICK_UNDERLINE:
            current_cursor = cursor_thick_underline;
            return true;
        default:
            return false;
    }
}

// get cursor scan lines

// parameters:  None

// returns:     The scan line
uint8_t cursorGet80x25CursorFirstScanLine() {return current_cursor.firstscanline;}
uint8_t cursorGet80x25CursorLastScanLine() {return current_cursor.lastscanline;}

// get the position of the cursor in the VGA buffer
// Parameters:  cursorpos -- The cursor position

// Returns:     The position in the buffer, or CURSOR_80X25_ERROR (65535)
uint16_t cursorCursorPos80x25ToLineal(struct cursor_pos_t cursorpos) {
    // valid parameters
    if (!cursorValidate80x25Cursor(cursorpos)) return CURSOR_80X25_ERROR;

    uint16_t lineal = (uint16_t) (((cursorpos.row) * (max80x25cols)) + (cursorpos.col));

    if (lineal > CURSOR_80X25_MAX_LINEAL_ADDRESS) return CURSOR_80X25_ERROR;

    return lineal;
}

// get the cursor position given the position in the VGA buffer
// Parameter:   lineal -- The buffer position

// Returns:     The curpos in cursor_pos_t format given a lineal address
struct cursor_pos_t cursorLineal80x25ToCursorpos(uint16_t lineal) {
    // make sure we got a good lineal address if it is too big, put cursor in lower right
    if (lineal > CURSOR_80X25_MAX_LINEAL_ADDRESS) return (struct cursor_pos_t){max80x25rows,max80x25cols};

    // create a cursor position object
    struct cursor_pos_t cursorpos;
    cursorpos.row = (lineal / max80x25cols);
    cursorpos.col = (lineal % max80x25cols);

    return cursorpos;
}

// initialize the cursor
// Parameters:  rows - number of rows for the screen
//              columns -- number of columns for the screen
//              lastscanline - last scan line available for the cursor

// Returns:     true if initialization is successful, false otherwise
bool initCursor(struct cursor_pos_t *cursor, size_t rows, size_t columns, uint8_t lastscanline) {
    // set the active cursor (optional)
    cursorPosition = cursor;

    // init max screen size
    max80x25cols = columns;
    max80x25rows = rows;

    // set initial cursor position 
    if (!cursorSet80x25CursorPosXY(0,0)) return false;

    // initialize the types of cursors
    cursor_block = (struct cursor_t) {0, lastscanline};
    cursor_underline = (struct cursor_t) {0,1};
    cursor_thick_underline = (struct cursor_t) {0, 2};

    bool rtncde = true;
    // set the default cursor
    rtncde &= cursorSetCursorType(CURSOR_BLOCK);

    return rtncde;
}