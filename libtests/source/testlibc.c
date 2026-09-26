// test the libc.a library

#include <bitmap.h>
#include <string.h>
#include <stdio.h>

bool testlibc() {
    // test 8 bit functions
    
    bool rtncde = true;
 
    // test 8 bit functions
    if (!isBitSet8(1,1)) {
        print("isBitSet8(1,1) failed\n\r");
        rtncde = false;
    }

    if (isBitSet8(0,1)) {
        print("isBitSet8(0,1) failed\n\r");
        rtncde = false;
    }
    if (clearBits8(1,1) != 0) {
        print("clearsBit8(1,1) failed\n\r");
        rtncde = false;
    }

    if (clearBits8(3,1) != 2) {
        print("clearsBit8(3,1) failed\n\r");
        rtncde = false;
    }

    if (setBits8(0,1) != 1) {
        print("setBits8(0,1) failed\n\r");
        rtncde = 0;
    }
 
    if (toggleBits8(0,1) != 1) {
        print("toggleBits8(0,1) failed\n\r");
        rtncde = false;
    }

    if (toggleBits8(1,1) != 0) {
        print("toggleBits8(1,1) faild\n\r");
        rtncde = 0;
    }

    // test 16 bit functions
    if (!isBitSet16(1,1)) {
        print("isBitSet16(1,1) failed\n\r");
        rtncde = false;
    }

    if (isBitSet16(0,1)) {
        print("isBitSet16(0,1) failed\n\r");
        rtncde = false;
    }

    if (setBits16(0,1) != 1) {
        print("setBits16(0,1) failed\n\r");
        rtncde = 0;
    }
 
    if (toggleBits16(0,1) != 1) {
        print("toggleBits16(0,1) failed\n\r");
        rtncde = false;
    }

    if (toggleBits16(1,1) != 0) {
        print("toggleBits16(1,1) faild\n\r");
        rtncde = 0;
    }
    
    // set 32 bit functions
    if (!isBitSet32(1,1)) {
        print("isBitSet32(1,1) failed\n\r");
        rtncde = false;
    }

    if (isBitSet32(0,1)) {
        print("isBitSet32(0,1) failed\n\r");
        rtncde = false;
    }

    if (setBits32(0,1) != 1) {
        print("setBits32(0,1) failed\n\r");
        rtncde = 0;
    }
 
    if (toggleBits32(0,1) != 1) {
        print("toggleBits32(0,1) failed\n\r");
        rtncde = false;
    }

    if (toggleBits32(1,1) != 0) {
        print("toggleBits32(1,1) faild\n\r");
        rtncde = 0;
    }

    return rtncde;
}