// keyboard functions

#ifndef _KBD_H
#define _KBD_H

// get keyboard input
// Parameters:  buffer -- the buffer for the input
//              bufflen -- length of the buffer
//              echo -- do we echo it to the screen
// Returns:     the number of characters retuned
extern int kbdInput(char* restrict buffer, int bufflen, bool echo);

#endif