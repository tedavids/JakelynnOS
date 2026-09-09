// store allocations of a page or more

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

#ifndef _PAGEALLOCTREE_H
#define _PAGEALLOCTREE_H

// get heap depth
extern uint16_t getPageTreeDepth();
// get number of nodes in the freelist
extern uint16_t getPageTreeFreelistSize();
// get number of nodes in the tree
extern uint32_t getPageTreeNumAlloc();

// allocate pages
void *allocPage(size_t size);
// free a page allocation
bool freePage(uint32_t ptr);

// initialize the heap tree
extern bool initPageTree();


#endif
