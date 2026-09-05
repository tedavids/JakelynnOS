// our implementation of memory functions

#include <string.h>


void *memchr(void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    unsigned char uc = (unsigned char)c;

    while (n--) {
        if (*p == uc) {
            return (void *)p;
        }
        p++;
    }

    return NULL;
}

// Compare two blocks of memory
//  If the first block is 'smaller' ie. via the value of each char it return -1
//  If both blocks are the same, it returns 0
//  If the first block is 'larger' it returns 1

// Parameters:  aptr -- the first block of memory
//              bptr -- the second block of memory
//              size -- the maximum number of blocks to compare

int memcmp(const void* aptr, const void* bptr, size_t size) {
    if (!aptr) return -999;
    if (!bptr) return 999;
    if (!size) return 0;
    
    const unsigned char* a = (const unsigned char*) aptr;
    const unsigned char* b = (const unsigned char*) bptr;

    for (size_t i = 0; i < size; i++) {
        // check if a is less than b
        if (a[i] < b[i]) 
            return -1;
        // check b is greater
        if (a[i] > b[i])
            return 1;
    } // for (size_t i = 0; i < size; i++)

    // both were the same
    return 0;
} //  int memcmp(const void* aptr, const void* bptr, size_t size)

//Copy one block of memmory to the other, the blocks may overlap

// Parameters:  dstptr -- Pointer to the destination
//              srcptr -- pointer to the source
//              size -- the number of bytes to copy

// Returns:     pointer to the destination, or nullpointer if there is an error

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
    if (!dstptr) return nullptr;
    if (!srcptr) return nullptr;
    if (!size) return nullptr;
    
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (const unsigned char*) srcptr;

    for(size_t i = 0; i < size; i++) 
        dst[i] = src[i];

    return dstptr;
} // void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size)

// move memory from one place to another, they may overlap

// Parameters:  dstptr -- a pointer to the destination addresss
//              srcptr -- a pointer to the source address
//              size -- the number of bytes to move

// Returns:     A pointer to the destination, or nullptr on an error

void* memmove(void* dstptr, const void* srcptr, size_t size) {
    if (!dstptr) return nullptr;
    if (!srcptr) return nullptr;
    if (!size) return nullptr;
    
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (const unsigned char*) srcptr;


    if (dst < src) {
        // overlap doesnt matter
        for (size_t i = 0; i < size; i++) 
            dst[i] = src[i]; 
    } else {
        // overlap might matter, so go backwards
        for (size_t i= size; i != 0; i--) 
            dst[i] = src[i];
    } // if (dst < src)

    return dstptr;
} // void* memmove(void* dstptr, const void* srcptr, size_t size) 

// set a block of memory to a value

// Parameters:  buffptr -- a pointer to the buffer you want to set the memory of
//              value -- The value you want the memory set to
//              size -- The number of bytes to set

void* memset(void* bufptr, int value, size_t size) {
    if (!bufptr) return nullptr;

    unsigned char* buf = (unsigned char*) bufptr;

    for (size_t i = 0; i < size; i++) 
        buf[i] = (unsigned char) value;

    return bufptr;
} // void* memset(void* bufptr, int value, size_t size)
