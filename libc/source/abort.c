// our abort command
#include <stdlib.h>
#include <stdio.h>

__attribute__((__noreturn__))
void abort(void) {
    // TODO: add proper kernel panic
    printf("Kernel: panic: abort()\n");
 
    while(true) __asm__ __volatile__ ("hlt");

} // void abort(void)