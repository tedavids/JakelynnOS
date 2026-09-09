// Page table functions

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

#ifndef _PAGETABLE_H
#define _PAGETABLE_H

// defines
#define PAGE_TABLE_START              0xFFC00000

// typedefs
typedef uint32_t pagetable_array_t[1024][1024];

extern pagetable_array_t * const    PAGETABLE;

// structs and typedefs
typedef uint32_t pte_t;                 // page table entry
typedef uint32_t pde_t;                 // page directory entry
typedef uint32_t page_table_t [1024];    // individual page table

// get the page directory entry from a virtual address
pde_t getPDEFromAddress(uint32_t virtaddr);

// get the page table enry from a virtual address
pde_t getPTEFromAddress(uint32_t virtaddr);

// Initialize the page directory
// this must happen after we do multiboot, because it clears page 0

// Parameters:  None

// Returns:     true if successful, false otherwise

extern bool initPageDirectory();

// get the virtual address of a particular page table
// Parameters:  pde -- The page directory entry of the table
// Returns:     The virtual address of the page table
extern uint32_t *getDirectoryTableVirtAddress(pde_t pde);

// get the virtual address of a particular page table
// Parameters:  pde -- The page directory entry of the table
// Returns:     The virtual address of the page table
extern uint32_t *getPageTableVirtAddress(pde_t pde, pte_t pte);

// get the page table physical address from a PDE,PDT
// Parameters:  pde - The page direcory entry
//              pte - The table table entry
// returns:     The physical address of the combination, or 0xFFFFFFF if failed

extern uint32_t getPageTablePhysAddress(uint32_t pde, uint32_t pte);

// set page table entry to a physical page
// Parameters:  virtaddr - the virtual address of the entry
//              physaddr -- the physical memory address to put in entry

// external for testing only
// get the next available kernel address
// Parameters:  None
// returns:     the next available kernel page, or zero if out of memory
uint32_t getNextAvailKernelPage();

// return:      true if successful, false if the page table entry already exists
extern bool setPageTableEntry(uint32_t virtaddr, uint32_t physaddr);

// clear page table entry to a physical page
// Parameters:  virtaddr - the virtual address of the entry

// return:      true if successful, false if the page table entry doesn't exists
extern bool clearPageTableEntry(uint32_t virtaddr);

// flush the Translation Lookaside Buffer (TLB)
// Parameters:  none
// returns;     None
extern void     FlushTLB();

// invalidate an individual page
// Parameters:  virtaddress -- The virtual address of the page to be invalidated
// Returns:     None
extern void     invalidatePage(void * virtaddress);

#endif