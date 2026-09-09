// These are functions to use for bit arrays

/* Copyright (C) 2026 Tom Davidson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _BITARRAY_H
#define _BITARRAY_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


// get the value of a bit (true or false)

// Parameters:  array -- The char array that has the bits
//              size -- The size of the bit array
//              index -- The index of the array you want to get
//              value -- The value of the bit

// Retuns:      True if the operation succeeds
//              value -- the value of the bit if the return code is true
//                          undefined if the return code is false
extern bool getBit(const unsigned char *array, size_t numbits, size_t index, bool *value);

// set the value for a bit (true or false) in a bit array/

// Parameters:  array -- The char array with the bits
//              size -- The size of the bit array
//              index -- The index of the array you want to get
//              oldValue -- The value of the bit before being changed

// Returns:     True if the operation succeeds
//              oldValue -- If return code is true, the value of the bit before it was chagned
//                              undefined otherwise

extern bool setBit(unsigned char *array, size_t numbits, size_t index, bool *oldValue);

// set the value fo a bit (true or false) in a bit array

// Parameters:  array -- The char array with the bits
//              numbits -- The size of the bit array
//              index -- The index of the array you want to get
//              oldValue -- The value of the bit before being changed

// Returns:     True if the operation succeeds
//              oldValue -- If return code is true, the value of the bit before it was chagned
//                              undefined otherwise

extern bool clearBit(unsigned char *array, size_t numbits, size_t index, bool *oldValue);

// set the value fo a bit (true or false) in a bit array

// Parameters:  array -- The char array with the bits
//              numbits -- The size of the bit array
//              index -- The index of the array you want to get
//              oldValue -- The value of the bit before being changed

// Returns:     True if the operation succeeds

extern bool toggleBit(unsigned char *array, size_t numbits, size_t index, bool *value);
#endif