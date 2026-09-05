// print the multiboot info
// this is part of the shell

#ifndef _PRINTMULTIBOOT_H
#define _PRINTMULTIBOOT_H

#include <stdint.h>

// print the memory map
// Parameters:  None

// Returns:     0 on normal completion

extern int printpmmap();

// print the command line
// Parameters:  None

// Returns:     0 on normal completion
extern int printcmdline();

#endif