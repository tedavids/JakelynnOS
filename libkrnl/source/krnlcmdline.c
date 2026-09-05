// process the kernel command line

#include <string.h>
#include <stdio.h>
#include <multiboot.h>
#include <krnlcmdline.h>

#define MAX_OPTIONS         2
#define MAX_OPTION_LEN      15
#define MIN_HEAP_PAGES      128
#define USE_VIDEO_DEFAULT   true


struct cmdline_options_t cmdLineOpts;

// process the heap size

// Parameter:    s -- the size of the heap
// returns:      true if successful, false otherwise
bool processHeapSize(const char * const __restrict s) {
    if (!s) return false;
    if (strlen(s) < 2) return false;

    uint32_t size = strtoui(s);

    // get UOM
    auto uom = s[strlen(s) - 1];
    switch (uom) {
        case 'P' :  break; // do nothing
        case 'K' :  // divide by K/page
                size /= 4;
                break;
        case 'M' :  // multiply by 128 pages
                size *= 256;
                break;
        case 'G' :  // multiply by 262144 pages
                size *= 262144;
                break;
        default:    return false;
    }   

    if (size < cmdLineOpts.heapSizePages) return false;

    cmdLineOpts.heapSizePages = size;

    return true;
}

// process kernel options
// Parameter:   s -- The heap size string, must start with -k
// returns:     true if successfull

bool processKernelOpt(const char * const __restrict s) {
    if (!s) return false;
    if (s[0] != '-') return false;
    if (s[1] != 'k') return false;
    
    // you did send me a kernel option to decode
    char * option;

    int start = 0;
    char buffer[MAX_OPTION_LEN];

    option = strtokbuf(&s[2]," =\t\n\r",buffer,sizeof(buffer),start, &start);
    if (!option) return false;
 
    // we now have an option and a value process them
    if (!strncmp(MAX_OPTION_LEN,option,"heap"))  {
        char * value = strtokbuf(&s[2]," =\t\n\r",buffer,sizeof(buffer),start, &start);
        if (!value) return false;
        return processHeapSize(value);
    }

    return false;
}

// process the kernel command line and set the command line options structure

// Parameters:  cmdline -- The command line

// returns:     true if all OK, false otherwise

bool processCommandLine(const char * const __restrict cmdline) {
    if (!cmdline) return false;

    bool rtncde = true;
    // initialize all options
    cmdLineOpts.heapSizePages = MIN_HEAP_PAGES;
    cmdLineOpts.useVideo = USE_VIDEO_DEFAULT;

    char   substring[MAX_OPTION_LEN];
    char   options[10][sizeof(substring)];
    int     start = 0;
    int     numoptions = 0;

    char* token = strtokbuf(cmdline," \t\r\n",substring,sizeof(substring),start,&start);
    while ((numoptions < MAX_OPTIONS) && token) {
        strncpy(options[numoptions++],token, sizeof(substring));
        token= strtokbuf(cmdline," \t\r\n",substring,sizeof(substring),start,&start);
    }

    // process the options
    for (int i = 0; i < numoptions; i++) {
        // make sure it starts with a dash
        if (options[i][0] == '-') {
            // check for static optins first
            if (!strncmp(sizeof(substring),options[i],"-no-video")) {
                cmdLineOpts.useVideo = false;
                continue;
            }

            // check for parameters that have options
            // we know it starts with a '-' so we can just check the second letter
            switch (options[i][1]) {
            case 'k':
                rtncde &= processKernelOpt(options[i]);
                break;
            default:
                break;
            }
        }
    }

    return rtncde;
}
