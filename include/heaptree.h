// Binary Tree keeping track of addresses passed out by the heap

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

#ifndef _HEAPTREE_H
#define _HEAPTREE_H

#include <stdint.h>
#include <stdbool.h>

struct heapTreeNode {
    struct heapTreeNode *leftChild;
    struct heapTreeNode *rightChild;
    uint32_t            address;        // addess we are keeing track of
    unsigned char       memstack;       // the memory stack it came from
    uint16_t            height;
};


// get heap depth
extern uint16_t getHeapTreeDepth(struct heapTreeNode *root);
// get number of nodes in the freelist
extern uint16_t getHeapTreeFreelistSize();
// get number of nodes in the tree
extern uint32_t getHeapTreeNumAlloc();

// get a node
// Parameter:   address - The address of the allocation

// Returns:     a pointer to the node with the allocation
extern struct heapTreeNode *getAllocTreeNode(uint32_t address);
// delete a node

// Parameter:   address -- The address of the allocation

// Returns:     Nothing
void deleteAllocTreeNode(uint32_t address);

// insert a node

// Parameters:  address -- the address of the allocation
//              memstack -- The type of the allocation

void insertAllocTreeNode(uint32_t address, unsigned char memstack);

// initialize the heap tree
extern bool initHeapTree();

#endif