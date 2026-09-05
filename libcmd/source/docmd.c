// do the command entered, this is effectively the shell

#include <string.h>
#include <printmultiboot.h>
#include <registers.h>
#include <stdio.h>


#include <docmd.h>

int doCommand(const char* command) {

    char    cmd[256];
    strncpy(cmd,command, sizeof(cmd));

    // remove trailing and leading garbage
    rtrimd(" \n\r",cmd);
    trimc(' ', cmd);
    trimc('\n',cmd);
    trimc('\r',cmd);

/*
    if (!strncmp(5,"mmap",cmd)) {
        return printmmap();
    }

    if (!strncmp(8,"cmdline",cmd)) {
        return printcmdline();
    }

    if (!strncmp(11,"registers",cmd)) {
        printAllRegisters();
        return 0;
    }

    return 0;
*/
    // buffer for token
    char buff[80];
    // starting point in buffer
    int start = 0;

    char *delims = " \n\r\t";

    char *token = strtokbuf(cmd,delims, buff, sizeof(buff), start, &start);

    // are we printing something?
    if (!strncmp((int)(strlen(token) + 1), "print", token)) {
        token = strtokbuf(cmd,delims,buff,sizeof(buff), start, &start);
        // if null we don't know what to print
        if (!token) return -1;
        
        if (!strncmp((int)(strlen(token) + 1),"pmmap",token)) {
            return printpmmap();
        } else if (!strncmp((int)(strlen(token) + 1),"cmdline", token)) {
            return printcmdline();
        } else if (!strncmp((int)(strlen(token) + 1), "registers", token)) {
            printAllRegisters();
            return 0;
        }
    }

    return 0;
}