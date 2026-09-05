// code for the string to something conversions (strtoxx)

#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>


int16_t  strtoi(const char * const __restrict s) {
    // check that string was passed
    if (!s) return 0;

    // get length
    int len = (int) strlen(s);
    // check for empty string
    if (!len) return 0;

    int i = 0;

    // check for leading negative sign
    bool isneg = false;
    if (s[i] == '-') {
        isneg = true; 
        i++;        // next char
    }

    long value = 0;
    int digits = 0;
    // get value (max 5 digits)
    for (i = 0; i < len; i++) {
        // not a digit
        if ((s[i] < '0') || (s[i] > '9')) break;
        value = (value * 10) + (s[i] - '0');
        // out of range
        if (digits++ > 5) break;
    }

    if (isneg) value = -value;

    if (value < MIN_INT) return MIN_INT;
    if (value > MAX_INT) return MAX_INT;

    return (int16_t) value;
}

uint16_t strtoui(const char * const __restrict s) {
    // check that string was passed
    if (!s) return 0;

    // get length
    int len = (int) strlen(s);
    // check for empty string
    if (!len) return 0;

    int i = 0;

    uint32_t value = 0;
    int digits = 0;
    // get value (max 5 digits)
    for (i = 0; i < len; i++) {
        // not a digit
        if ((s[i] < '0') || (s[i] > '9')) break;
        value = (value * 10) + (uint16_t) (s[i] - '0');
        // out of range
        if (digits++ > 5) break;
    }

    if (value > MAX_UINT) return MAX_UINT;

    return (uint16_t) value;
}

int32_t strtol(const char * const __restrict s) {
    // check that string was passed
    if (!s) return 0;

    // get length
    int len = (int) strlen(s);
    // check for empty string
    if (!len) return 0;

    int i = 0;

    // check for leading negative sign
    bool isneg = false;
    if (s[i] == '-') {
        isneg = true; 
        i++;        // next char
    }

    int64_t value = 0;
    int digits = 0;
    // get value (max 5 digits)
    for (i = 0; i < len; i++) {
        // not a digit
        if ((s[i] < '0') || (s[i] > '9')) break;
        value = (value * 10) + (uint16_t) (s[i] - '0');
        // out of range
        if (digits++ > 5) break;
    }

    if (isneg) value = -value;

    if (value < MIN_LONG) return MIN_LONG;
    if (value > MAX_LONG) return MAX_LONG;

    return (int32_t) value;
}


uint32_t strtoul(const char * const __restrict s) {
    // check that string was passed
    if (!s) return 0;

    // get length
    int len = (int) strlen(s);
    // check for empty string
    if (!len) return 0;

    int i = 0;

    uint64_t value = 0;
    int digits = 0;
    // get value (max 5 digits)
    for (i = 0; i < len; i++) {
        // not a digit
        if ((s[i] < '0') || (s[i] > '9')) break;
        value = (value * 10) + (uint16_t) (s[i] - '0');
        // out of range
        if (digits++ > 11) break;
    }

    if (value > MAX_ULONG) return MAX_ULONG;

    return (uint32_t) value;
}

