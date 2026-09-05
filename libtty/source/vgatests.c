// Video tests

#include <stdio.h>

#include <vga.h>

// test VGA
bool vgatest() {
    bool rtncde = true;

    if (vgaGetMaxCols() != 80) {
        printf("vgaGetMaxCols failed.  Did not return 80, returned: %ul\n\r",vgaGetMaxCols());
        rtncde = false;
    }
    if (vgaGetMaxRows() != 25) {
        printf("vgaGetMaxRows failed. Did not return 25, returned: %ul\n\r",vgaGetMaxRows());
        rtncde = false;
    }

    if (20 != vgaCreateTextColor(VGA_COLOR_RED,VGA_COLOR_BLUE)) {
        printf("vgaCreateTextColor failed on RED/BLUE(16) returned %ub\n\r",vgaCreateTextColor(VGA_COLOR_RED,VGA_COLOR_BLUE));
        rtncde = false;
    }
    if (vgaCreateText('j',7) != 0x76a) {
        printf("vgaCreateText failed, shoudl be 0x76a, returned: 0x%xw\n\r",vgaCreateText('j',7));
        rtncde = false;
    }
    if (vgaGetTextColor() != 7) {
        printf("vgaGetTextColor failed, shoudl have retuned 7, returned: %ub\n\r",vgaGetTextColor());
        rtncde = false;
    }

    return rtncde;
}