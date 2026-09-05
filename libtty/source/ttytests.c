// test TTY functions
#include <stdio.h>

#include <tty.h>

bool ttytest() {
    bool rtncde = true;

    // make sure all is initialized
    char *shortstring = "This is a short string\n\r";
    char *longstring = "This is a long string that should wrap around, if it doesn't then there is some sort of issue. this should wrap\n\r";

    // i can test just ttyWriteString because it uses ttyWrite
    ttyWriteString(shortstring);
    ttyWriteString(longstring);

    return rtncde;
}
