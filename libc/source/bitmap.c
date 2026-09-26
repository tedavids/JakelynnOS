// bitmap operations

#include <bitmap.h>

// Set bits, 8 bit version
// Parameters:  target - the object to change
//              bitmap - the bitmap
// Returns:     The new target
uint8_t setBits8(uint8_t target, uint8_t bitmap) {
    return target | bitmap;
}

// Clear bits, 8 bit version
// Parameters:  The object to change
//              bitmap - the bitmap
// Returns:     the new target
uint8_t clearBits8(uint8_t target, uint8_t bitmap) {
    return target & ~bitmap;
}

// are all bits set, 8 bit version
// Parameters:  target - The number to check
//              bitmap - the bits to check
bool isBitSet8(uint8_t target, uint8_t bitmap) {
    return ((target & bitmap) == bitmap);
}

// Toggle bits, 8 bit version
// Parameters:  target - the number to change
//              bitmap - containts the bits to toggle
// Returns:     the new number
uint8_t toggleBits8(uint8_t target, uint8_t bitmap) {
    return target ^ bitmap;
}

// Set bits, 16 bit version
// Parameters:  target - the object to change
//              bitmap - the bitmap
// Returns:     The new target
uint16_t setBits16(uint16_t target, uint16_t bitmap) {
    return target | bitmap;
}

// Clear bits, 16 bit version
// Parameters:  The object to change
//              bitmap - the bitmap
// Returns:     the new target
uint16_t clearBits16(uint16_t target, uint16_t bitmap) {
    return target & ~bitmap;
}

// are all bits set, 16 bit version
// Parameters:  target - The number to check
//              bitmap - the bits to check
bool isBitSet16(uint16_t target, uint16_t bitmap) {
    return ((target & bitmap) == bitmap);
}

// Toggle bits, 16 bit version
// Parameters:  target - the number to change
//              bitmap - containts the bits to toggle
// Returns:     the new number
uint16_t toggleBits16(uint16_t target, uint16_t bitmap) {
    return target ^ bitmap;
}

// Set bits, 32 bit version
// Parameters:  target - the object to change
//              bitmap - the bitmap
// Returns:     The new target
uint32_t setBits32(uint32_t target, uint32_t bitmap) {
    return target | bitmap;
}

// Clear bits, 32 bit version
// Parameters:  The object to change
//              bitmap - the bitmap
// Returns:     the new target
uint32_t clearBits32(uint32_t target, uint32_t bitmap) {
    return target & ~bitmap;
}

// are all bits set, 32 bit version
// Parameters:  target - The number to check
//              bitmap - the bits to check
bool isBitSet32(uint32_t target, uint32_t bitmap) {
    return ((target & bitmap) == bitmap);
}

// Toggle bits, 32 bit version
// Parameters:  target - the number to change
//              bitmap - containts the bits to toggle
// Returns:     the new number
uint32_t toggleBits32(uint32_t target, uint32_t bitmap) {
    return target ^ bitmap;
}

