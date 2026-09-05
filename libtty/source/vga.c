// vga.c  VGA specific functions
#include <kernel>
#include "80x25cursor.h"

#include "vga.h"

// defines

#define VGA_CTRL_REGISTER           0x3D4
#define VGA_DATA_REGISTER           0x3D5
#define VGA_CTRL_DATA_REGISTER_LOW  0x0F
#define VGA_CTRL_DATA_REGISTER_HIGH 0x0E
#define VGA_CURSOR_ON_OFF           0x0A
#define VGA_CURSOR_FIRST_LINE       0x0A
#define VGA_CURSOR_LAST_LINE        0x0B
#define VGA_CURSOR_OFF              0x20

#define VGA_80X25_ROWS              25
#define VGA_80X25_COLUMNS           80



typedef bool (*validateCursorPos_fn)(struct cursor_pos_t cursorposition);
typedef size_t (*maxRows_fn)();
typedef uint8_t (*getScanline_fn)();
typedef struct cursor_pos_t (*linealToCursorPos_fn)(uint16_t lineal);
typedef uint16_t (*cursorposToLineal_fn)(struct cursor_pos_t cursorpos);
typedef size_t (*maxLinealAddress_fn)();

static moveCursor_fn   cursorUp;
static moveCursor_fn   cursorDown;
static moveCursor_fn   cursorLeft;
static moveCursor_fn   cursorRight;

static validateCursorPos_fn   vgaValidateCursorPos;

static maxRows_fn      getMaxRows;
static maxRows_fn      getMaxCols;

static getScanline_fn  getFirstScanline;
static getScanline_fn  getLastScanline;

static linealToCursorPos_fn linealToCursorPos;
static cursorposToLineal_fn cursorPosToLineal;

static maxLinealAddress_fn maxLinealAddress;

static uint16_t cursorError;

static uint16_t* vgaMemory = (uint16_t *) VGA_MEMORY_START; // physical address of our memory
static uint8_t  textColor;

// internal functions

size_t vgaGetRowLinealOffset(size_t row) {
    // validate parameters
    if (row >= vgaGetMaxRows()) return false;

    // send back the address
    return row * getMaxCols();
}

// get max row or column size
// Parameters:  None
// Returns:     The ow or column size

size_t vgaGetMaxRows() {return VGA_80X25_ROWS;}
size_t vgaGetMaxCols() {return VGA_80X25_COLUMNS;}

// get the color of a vga color as a byte

// inputs:  fg -- the color of the text
//          bg -- the background color

// returns: the color as a byte

inline uint8_t vgaCreateTextColor( enum vga_color fg, enum vga_color bg) 
{
    return (uint8_t) (fg | (bg << 4));
}

// get the entry as it will be presented to the VGA adaptor

// inputs:  uc -- the character to print
//          color -- the foreground and background colors to use (from vga_entry_color)

// returns: a 16-bit value usable as a VGA buffer entry

inline uint16_t vgaCreateText(unsigned char uc, uint8_t color) 
{
    return (uint16_t) uc | ((uint16_t) color << 8);
}

// blank a row to the background color
// Parameters:  None

// Returns:     true if successful, false otherwise
bool vgaClearRow(size_t row) {
    // check parameters
    if (row >= getMaxRows()) return false;

    // get lineal address at start of row
    auto linealoffset = vgaGetRowLinealOffset(row);

    for (size_t i = 0; i < getMaxCols(); i++) {
        vgaMemory[linealoffset + i] = vgaCreateText(' ',textColor);
    }

    return true;
}

// scroll row up one line
// Parameters:  row -- The row to scroll up
// Returns      true if successful, false otherwise
bool vgaScrollRowUp(size_t row) {
    // check parameter
    if (row >= getMaxRows()) return false;

    // first row can't scroll, it just blanks
    if (row == 0) {
        return vgaClearRow(row);
    }

    // copy next row up one row
    auto aboveaddress = vgaGetRowLinealOffset(row -1);
    auto rowaddress = vgaGetRowLinealOffset(row);

    // copy the row up
    for (size_t i = 0; i < vgaGetMaxCols(); i++) {
        vgaMemory[aboveaddress + i] = vgaMemory[rowaddress + i];
    }
    // blank the row (it's now up 1)
    return vgaClearRow(row);
}

// scroll row up one line
// Parameters:  row -- The row to scroll down
// Returns      true if successful, false otherwise
bool vgaScrollRowDown(size_t row) {
    // check parameters
    if (row >= vgaGetMaxRows()) return false;

    // row can't scroll just clear it
    if (row == vgaGetMaxRows() - 1) {
        return vgaClearRow(row);
    }
    // copy our row down
    auto rowaddress = vgaGetRowLinealOffset(row);
    auto belowaddress = vgaGetRowLinealOffset(row + 1);

    for (size_t i = 0; i < vgaGetMaxCols(); i++) {
        vgaMemory[belowaddress + i] = vgaMemory[rowaddress + i];
    }

    // blank the row
    return vgaClearRow(row);
}

// scroll the entire screen
// Parameters:  none
// Returns:     true if successful, false otherwise
bool vgaScrollAll() {
    bool rtncde = true;

    for (size_t row = 0; row < vgaGetMaxRows(); row++) {
        rtncde &= vgaScrollRowUp(row);
    }

    return rtncde;
}

// clear the entire screen to the default color
// Parameters: None
// Return:      true if successful, false otherwise;
bool vgaClearScreen() {
    // just clear the screen
    for (size_t i = 0; i < maxLinealAddress(); i++) {
        vgaMemory[i] = vgaCreateText(' ', textColor);
    }

    return true;
}

// enable the vga cursor
// Parameters:  type the cursor type
// Returns:     true if successful, false otherwise 
bool vgaEnableCursor(struct cursor_t type) {
    // validate parameters
     if ((type.firstscanline > 15) || (type.firstscanline > type.lastscanline)) return false;
    if (type.lastscanline > 15) return false;

    putPortByte(VGA_CTRL_REGISTER,VGA_CURSOR_FIRST_LINE);
    putPortByte(VGA_DATA_REGISTER, getFirstScanline());

    putPortByte(VGA_CTRL_REGISTER, VGA_CURSOR_LAST_LINE);
    putPortByte(VGA_DATA_REGISTER, getLastScanline());

    return true;
}

// Disable the cursor
// Parameters:  None
// Returns None
void vgaDisableCursor() {
    putPortByte(VGA_CTRL_REGISTER, VGA_CURSOR_ON_OFF);
    putPortByte(VGA_DATA_REGISTER, VGA_CURSOR_OFF);
}

// put a character on the screen
// Parameters:  color -- The color code for the character
// Returns:     true if the put is successful, false otherwise

bool vgaPutChar(uint8_t color, uint8_t character, struct cursor_pos_t position) {
    if (!vgaValidateCursorPos(position)) return false;

    auto linealoffset = cursorPosToLineal(position);
    if (linealoffset == CURSOR_80X25_ERROR) return false;
    auto text = vgaCreateText(character,color);
    vgaMemory[linealoffset] = text;
    return true;
}

// set the background color
// Paremeters:  color -- new background color
// Returns:     old background color
uint8_t vgaSetTextColor(uint8_t color) {
    auto oldcolor = textColor;

    textColor = color;

    return oldcolor;
}

// get background color
// Parameters:  None
// Returns:     The background color
uint8_t vgaGetTextColor() {
    return textColor;
}

bool initVGA(struct cursor_pos_t *cursor) {
    bool rtncde = true;

    // set up our functions these will eventually be moved to a function so we can support multiple resolutions

    rtncde &= initCursor(cursor,25,80,15);

    cursorUp = cursorMove80x25CursorUp;
    cursorDown = cursorMove80x25CursorDown;
    cursorLeft = cursorMove80x25CursorLeft;
    cursorRight = cursorMove80x25CursorRight;

    vgaValidateCursorPos = cursorValidate80x25Cursor;

    getMaxRows = cursorGetMax80x25Rows;
    getMaxCols = cursorGetMax80x25Cols;

    getFirstScanline = cursorGet80x25CursorFirstScanLine;
    getLastScanline = cursorGet80x25CursorLastScanLine;

    linealToCursorPos = cursorLineal80x25ToCursorpos;
    cursorPosToLineal = cursorCursorPos80x25ToLineal;

    maxLinealAddress = cursorGetMax80x25LinealOffset;

    cursorError = CURSOR_80X25_ERROR;

    textColor = vgaCreateTextColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    rtncde &= vgaEnableCursor(*cursorGetCurrentCursor());

    return rtncde;
}