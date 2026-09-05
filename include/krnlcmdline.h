// process the kernel command line

#ifndef _KRNLCMDLINE_H
#define _KRNLCMDLINE_H

#include <stdint.h>
#include <stdbool.h>

struct cmdline_options_t {
    uint32_t    heapSizePages;
    bool        useVideo;
};

extern struct cmdline_options_t cmdLineOpts;

// process the kernel command line and set the command line options structure

// Parameters:  cmdline -- The command line

// returns:     true if all OK, false otherwise

extern bool processCommandLine(const char * const __restrict cmdline);


#endif