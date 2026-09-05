// test that the cursor is working
#include <stdio.h>

#include <80x25cursor.h>

bool cursortest() {
    struct cursor_pos_t testcursor;
    struct cursor_pos_t *oldCursor = cursorGetActiveCursor();

    bool rtncde = true;

    // we sould have had an active cursor
    if (!oldCursor) {
        print("cursorGetActiveCursor failed.  nullptr returned\n\r");
        rtncde = false;
    }
    // test init cursor
    rtncde &= initCursor(&testcursor, 25, 80, 15);
    if (!rtncde) {
        print("initCursor() failed\n\r");
        if (testcursor.row != 0) {
            printf("Init of cursor.row failed, row = %ul\n\r", testcursor.row);
            rtncde = false;
        } 
        if (testcursor.col != 0) {
            printf("Init of cursor.col failed. col = %ul\n\r", testcursor.col);
            rtncde = false;
        }
        if (cursorGetMax80x25Rows() != 25) {
            printf("cursorGetMax80x25Rows() failed.  result = %ul\n\r",cursorGetMax80x25Rows());
            rtncde = false;
        }
        if (cursorGetMax80x25Cols() != 79) {
            printf("cursorGetMax80x25Cols() failed.  result = %ul\n\r",cursorGetMax80x25Cols());
            rtncde = false;
        }
        if (cursorGet80x25CursorFirstScanLine() != 0) {
            printf("cursorGet80x25CursorFirstScanLine() failed.  return = %ul\n\r",cursorGet80x25CursorFirstScanLine());
            rtncde = false;
        }
        if (cursorGet80x25CursorLastScanLine() != 15) {
            printf("cursorGet80x25CursorLastScanLine() failed.  return = %ul\n\r",cursorGet80x25CursorLastScanLine());
            rtncde = false;
        }
        // test set position
        if (cursorSet80x25CursorPosXY(0,80)) {
            print("cursorSet80x25CursorPosXY(0,80) returned true\n\r");
            rtncde = false;
        }
        if (cursorSet80x25CursorPosXY(25,0)) {
            print("cursorSet80x25CursorPosXY(25,0) returned true\n\r");
            rtncde = false;
        }
        if (!cursorSet80x25CursorPosXY(5,5)) {
            print("cursorSet80x25CursorPosXY(5,5) failed\n\r");
            rtncde = false;
        }
        // test lineal address
        if (cursorGetMax80x25LinealOffset() != 2000) {
            printf("cursorGetMax80x25LinealOffset() did not return 20000, returned %ul\n\r", cursorGetMax80x25LinealOffset());
            rtncde = false;
        }
        cursorSet80x25CursorPosXY(24,79);
        if (cursorCursorPos80x25ToLineal(testcursor) != 2000) {
            printf("cursorSet80x25Pos80x25ToLineal() did not return 2000, returned: %ul\n\r",cursorCursorPos80x25ToLineal(testcursor));
            rtncde = false;
        }
        struct cursor_pos_t tempcursor = cursorLineal80x25ToCursorpos(2000);
        if ((tempcursor.row != 24) || (tempcursor.col != 79)) {
            print("cursorLineal80x25ToCursorpos(2000) did not work\n\r");
            rtncde = false;
        }
        if (testcursor.col != 79) {
            print("cursorSet80x25PosXY() did not work on column\n\r");
            rtncde = false;
        }
        if (testcursor.row != 24) {
            print("cursorSet80x25PosXY() did not work on row\n\r");
            rtncde = false;
        }
        if (!cursorSet80x25CursorToBeginOfLine()) {
            print("cursorSet80x25CursorToBeginOfLine() returned false\n\r");
            rtncde = false;
        } else {
            if (testcursor.col != 0) {
                printf("cursorSet80x25CursorToBeginOfLine returned non zero value\n\r");
            }
        }
        // move tests
        cursorSet80x25CursorPosXY(5,5);
        if (cursorMove80x25CursorUp(true)) {
            print("cursorMove80x25CursorUp returned scroll when it shouldn't have\n\r");
            rtncde = 0;
        }
        if (tempcursor.row != 4) {
            print("cursorMove80x25CursorUp did not change row appropriately\n\r");
            rtncde = false;
        }
        if (tempcursor.col != 5) {
            print("cursorMove80x25CursorUp changed column\n\r");
            rtncde = false;
        }
        // row = 4, col = 5
        if (cursorMove80x25CursorDown(true)) {
            print("cursorMove80x25CursorUp returned scroll when it shouldn't have\n\r");
            rtncde = false;
        }
        if (tempcursor.row != 5) {
            print("cursorMove80x25CursorUp() did not change row properly\n\r");
            rtncde = false;
        }
        if (tempcursor.col != 5) {
            print("cursorMove80x25CursorUp() chnaged column\n\r");
            rtncde = false;
        }
        // row = 5, col = 5
        if (cursorMove80x25CursorRight(true)) {
            print("cursorMove80x25CursorRight() returned scroll when it shouldn't have\n\r");
            rtncde = false;
        }
        if (tempcursor.row != 5) {
            print("cursorMove80x25CursorRight() changed row when it shoudn't have\n\r");
            rtncde = false;
        }
        if (tempcursor.col != 6) {
            print("cursorMove80x25CursorRight() changed changed column wrong\n\r");
            rtncde = false;
        }
        // row = 5, col = 6
        if (cursorMove80x25CursorLeft(true)) {
            print("cursorMove80x25CursorLeft() indicated scroll when it shouldn't have\n\r");
            rtncde = false;
        }
        if (tempcursor.row != 5) {
            print("cursorMove80x25CursorLeft() changed row inappropriately\n\r");
            rtncde = false;
        }
        if (tempcursor.col != 5) {
            print("cursorMove80x25CursorLeft did not change cursor appropriately\n\r");
            rtncde = false;
        }
        // place cursor in top right
        cursorSet80x25CursorPosXY(0,0);
        if (!cursorMove80x25CursorLeft(true)) {
            printf("cursorMove80x25CursorLeft did not indicte scroll appropriately\n\r");
            rtncde = false;
        }
        if (tempcursor.col != 79) {
            printf("cursorMove80x25Cursorleft set column wrong\n\r");
            rtncde = false;
        }
        if (tempcursor.row != 0) {
            printf("cursorMove80x25Cursorleft set row when it shoudln't have\n\r");
            rtncde = false;
        }
        // row = 0, col = 79
        if (!cursorMove80x25CursorLeft(true)) {
            print("cursorMove80x25CursorLeft did not indicate scroll when it should have\n\r");
            rtncde = false;
        }
        if (tempcursor.col != 0) {
            print("cursorMove80x25CursorLeft changed column wrong\n\r");
            rtncde = 0;
        }
        // row = 0, col = 0
        cursorSet80x25CursorPosXY(24,0);
        if (!cursorMove80x25CursorDown(true)) {
            print("cursorMove80x25CursorDown did not indicate scroll\n\r");
            rtncde = false;
        }
        if (tempcursor.row != 24) {
            print("cursorMove80x25CursorDown changed row inappropriately\n\r");
            rtncde = false;
        }
        cursorSet80x25CursorPosXY(24,79);
        if (!cursorMove80x25CursorRight(true)) {
            print("cursorMove80x25CursorRight did not indicate scroll\n\r");
            rtncde = false;
        }
        if (tempcursor.col != 0) {
            print("cursorMove80x25CursorRight did not change col correctly\n\r");
            rtncde = false;
        }

        // check cursor types
        if (cursorGetCurrentCursorType() != CURSOR_BLOCK) {
            print("cursorGetCurrentCursorType did not return block cursor\n\r");
            rtncde = false;
        }
        if (cursorSetCursorType(4)) {
            print("cursorSetCursorType accepted invalid value\n\r");
            rtncde = false;
        }
    }

    if (cursorSetActiveCursor(oldCursor) == nullptr) {
        print("cursorSetActiveCursor() returned nullptr\n\r");
        rtncde = false;
    }
    return rtncde;
}