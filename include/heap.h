// heap.h heap functions and types

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

#ifndef _HEAP_H
#define _HEAP_H

#include <stdint.h>


// allocation functions
// allocate space for the kernel

// Parameters:  size -- number of bytes to allocate (max 0x8000, 8 pages)

// return:      void pointer to the memory allocated
extern void *kmalloc(size_t size);

// allocate initialized space for the kernel

// Parameters:  size -- number of bytes to allocate (max 0x8000, 8 pages)

// returns:     void pointer to the memory allocated

extern void *kcalloc(size_t size);

// Free space allocated by kmalloc/kcalloc
extern void kfree(void * addr);

// stat functions
// get the number of allocations for the heap

// Returns: The total number of allocations in the heap
extern uint32_t getHeapSize();

// initialize the heap

// Returns: true if the heap initializes successfully

extern bool initHeap();



#endif