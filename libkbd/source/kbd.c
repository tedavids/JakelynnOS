// handle keyboard input

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>

#include <80x25cursor.h>
#include <ps2kbd.h>
#include <stdio.h>

#include <kbd.h>

// blank prior character on the screen
// Parameters:  echo - are we printing to the screen
// Returns:     nothing
void blankPriorChar(bool echo) {
    // if we aren't echoing don't do it
    if (!echo) return;

    struct cursor_pos_t *pos = cursorGetActiveCursor();
    // blank out prior character
    if (pos->col != 0) {
        pos->col--;
    }
    cursorSetActiveCursor(pos);
    //blank the character
    print(" ");
    // put character over space
    if (pos->col != 0) {
        pos->col--;
    }
}

// get keyboard input
// Parameters:  buffer -- the buffer for the input
//              bufflen -- length of the buffer
// Returns:     the number of characters retuned
extern int kbdInput(char* restrict buffer, int bufflen, bool echo) {
   // check parameters
    if (!buffer) return 0;
    if (!bufflen) return 0;

    uint16_t key = 0;
    int cmdidx = 0;
    char ascii = 0;

    // wait for a key (if necessary)
    while (kbdBufferEmpty());

    // do until we get to the end of the command (enter) or ESC is pressed
    // minus 2 because we need space for eh null
    while ((((char)key != '\n') && ((char)key != KBD_KEY_ESC)) && (cmdidx < (bufflen -2))) {
        key = kbdGetKey();
        ascii = (char) key;
        // if its valid ascii (ignore control chars kinda)) TODO: make more bulletproof
        switch (ascii) {
            // non-printing characters
            case 0: 
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                break;

            // backspace
            case 8: 
                if (cmdidx > 0) {
                    // move cursor back 1 space
                    if (cmdidx) {
                        blankPriorChar(echo);
                    }
                    cmdidx--;
                    buffer[cmdidx] = '\0';
                }
                    break;
            // newline
            case 0xa:
                buffer[cmdidx] = '\0';
                break;
            case KBD_KEY_ESC:
                buffer[0] = '\0';
                return cmdidx;
            default:    
                buffer[cmdidx++] = (char) key;
                // ignore non-printing characters
                if ((ascii > 0x1f) && (ascii < 127)) {
                    if(echo) printf("%c",ascii);
                }
            break;
        }


        // do we wait for the next key
        if ((ascii != '\n') && (ascii != KBD_KEY_ESC) && (cmdidx < (bufflen -2))) {
            while (kbdBufferEmpty());
        }
    }

    // terminate
    buffer[cmdidx] = '\0';
    

    return cmdidx;
}