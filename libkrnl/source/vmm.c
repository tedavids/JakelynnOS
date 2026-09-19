// this is the Virtual Memory Manager (VMM) implementation

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

#include <stdint.h>
#include <stdbool.h>

#include <vmm.h>

// defines

// Page directory defines
#define PGDIR_PRESENT           0b1
#define PGDIR_READ_WRITE        0b10
#define PGDIR_USER_MEM          0b100
#define PGDIR_PAGE_WRITE_THRU   0b1000
#define PGDIR_CACHE_DISABLE     0b10000
#define PGDIR_4K                0b1000000
#define PGDIR_GLOBAL            0b10000000
#define PGDIR_PAGE_ATTR_TBL     0b100000000

#define PDE_SHIFT               22

// Virtual memory defines
#define VIRT_MEMORY_PRESENT         0b1
#define VIRT_MEMORY_READ_WRITE      0b10
#define VIRT_MEMORY_USER_MEM        0b100
#define VIRT_MEMORY_PAGE_WRITE_THRU 0b1000
#define VIRT_MEMORY_CACHE_DISABLE   0b10000
#define VIRT_MEMORY_4K              0b10000000
#define VIRT_MEMORY_GLOBAL          0b100000000
#define VIRT_MEMORY_KERNEL          0b1000000000
#define VIRT_MEMORY_IN_USE          0b10000000000
#define VIRT_MEMORY_SWAPABLE        0b100000000000
#define VIRT_MEMORY_SWAPPED_OUT     0b1000000000000
         
#define MEM_PAGE_TABLE_SIZE         (MEM_PAGE_SIZE * 1024)

// macros
#define PGDIR_ATTR_MASK(pde)        (pde & 0x119F)

#define PGTBL_ATTR_MASK(pte)        (pte & 0x019F)

// externals
extern uint32_t page_directory[1024];

// typedefs/structs

typedef uint32_t VirtPageOff_t;
typedef uint8_t  PgDirProp_t[1024];
typedef uint16_t PageTableArray[1024][1024];

// variables 

// page directory

PgDirProp_t vmmPgDirProp;
PgDirProp_t * vmmPgDirPID = &vmmPgDirProp;

// Virtual memory
PageTableArray vmmPageTableProp;
PageTableArray * vmmPageTablePID = &vmmPageTableProp;     // pointer current PID page directory properties


//shared global functions

// get an address from a page directory entry and page table entry
// Parameters:  pde - the page directory entry
//              pte - the page table entry
// Returns:     The address of the page matching the entries, or MEM_ADDRESS_ERROR
MemAddr_t AddrFromPdePte(pde_t pde, pte_t pte) {
    // check parameters
    if ((pde > 1023) || (pte > 1023)) return MEM_ADDRESS_ERROR;

    return (pde * MEM_PAGE_TABLE_SIZE) + (pte * MEM_PAGE_SIZE);
}

// get Page directory entry index from an address
// Parameters:  address - The address to get the pde from
// Returns:     the pde for the address
pde_t PdeFromAddress(MemAddr_t address) {
    return (address >> PDE_SHIFT) & 0x3FF;
}

// get Page table entry index from an address
// Parameters:  address - The address to get the pde from
// Returns:     the pde for the address
pte_t PteFromAddress(MemAddr_t address) {
    return (address / (MEM_PAGE_SIZE * 1024));
}


// PPPPPP      A      GGGGGGGG EEEEEEE     DDDDDD  IIII RRRRRR
// PP   PP    AAA     GG       EE          DD   DD  II  RR   RR
// PPPPPP    AA AA    GG       EEEEE       DD   DD  II  RR   RR
// PP       AAAAAAA   GG  GGGG EE          DD   DD  II  RRRRRR
// PP      AA     AA  GG    GG EE          DD   DD  II  RR  RR 
// PP     AA       AA GGGGGGGG EEEEEEE     DDDDDD  IIII RR   RR

// Internal functions

// Change the properties to an Attribute mask
// Parameters:  idx - index of property array
// Returns:     a mask of the properties or 0 if errnor
uint32_t pgdirPropToAttr(uint32_t idx) {
    // check index 
    if (idx > 1023) return 0;
    // if it isn't present, you shouldn't be checking
    if (!((*vmmPgDirPID)[idx] & PGDIR_PRESENT)) return 0;

    uint32_t mask = PGDIR_PRESENT;
    if ((*vmmPgDirPID)[idx] & PGDIR_READ_WRITE) mask += PGDIR_READ_WRITE;
    if ((*vmmPgDirPID)[idx] & PGDIR_USER_MEM) mask += PGDIR_USER_MEM;
    if ((*vmmPgDirPID)[idx] & PGDIR_PAGE_WRITE_THRU) mask += PGDIR_PAGE_WRITE_THRU;
    if ((*vmmPgDirPID)[idx] & PGDIR_CACHE_DISABLE) mask += PGDIR_CACHE_DISABLE;
    if ((*vmmPgDirPID)[idx] & PGDIR_4K) mask += PGDIR_4K;
    if ((*vmmPgDirPID)[idx] & PGDIR_GLOBAL) mask += 0b100000000;
    if ((*vmmPgDirPID)[idx] & PGDIR_PAGE_ATTR_TBL) mask += 0b1000000000000;

    return mask;  
}

// Evaluate page directory entry and see if it matches the attributes
// ignores dirty and accesses bits
// Parameters:  idx - the page directories index
// Returns:     True if OK, false otherwise
bool pgdirEvalPde(uint32_t idx) {
    // check parameters
    if (idx > 1023) return false;

    return (PGDIR_ATTR_MASK(page_directory[idx]) == pgdirPropToAttr(idx));
}


// Set page directory entry's attributes to as they should be
// leaves dirty and accessed bits as they were
// Parameters:  idx - The page directory index
//              invalidate - Set page directory entries to as they should be
// Returns:     true if successful, false otherwise
bool pgdirFixPde(uint32_t idx, bool invalidate) {
    // check parameters
    if (idx > 1023) return false;
    // does it need changing
    if (pgdirEvalPde(idx)) return true;

    // it needs changing
    uint32_t mask = (*pgdirPropToAttr)(idx);  // initial mask
    mask |= 0b100000; // set accessed
    mask |= 0b1000000; // set dirty
    mask |= 0xFFFFE000; // high bits

    page_directory[idx] &=  mask;
    if (invalidate) {
        invalidatePage(&page_directory[idx]);
    }

    return true;
}

// sync page directory to its attributes and flush
// Parameters:  None
// Returns:     True if there were no errors, false if there were errors
bool pgdirSyncAttr(bool fix) {
    
    bool rtncde = true;

    for (uint32_t i = 0; i < 1024; i++) {
        // does it exist, if not the rest doesn't matter
        if (page_directory[i] & PGDIR_PRESENT) {
            // is it out of sync
            if (!pgdirEvalPde(i)) {
                if (fix) {
                    pgdirFixPde(i, true);
                }
                rtncde = false;
            }
        }
        
    }

    return rtncde;
}

// externals

// Validate the page directory attributes with the actual page directory
// Parameters:  sync -- Resync attributes from actual page directory
// Returns:     true if in sync, false otherwise, sync should ensure a true return
extern bool pgdirValidatePgDir(bool sync) {
    return pgdirSyncAttr(sync);
}

// Initialize the Page Directory structures
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool initPgDir() {
    bool rtncde = true;

    for (uint32_t i = 0; i < 1024; i++) {
        
    }
    return rtncde;
}

// VV         VV IIIIII RRRRRR  TTTTTTTT   MM     MM EEEEEE MM     MM
//  VV       VV    II   RR   RR    TT      MMM   MMM EE     MMM   MMM
//   VV     VV     II   RR   RR    TT      MMMM MMMM EEEEE  MMMM MMMM
//    VV   VV      II   RRRRRR     TT      MM  M  MM EE     MM  M  MM
//     VV VV       II   RR  RR     TT      MM     MM EE     MM     MM
//      VVV      IIIIII RR   RR    TT      MM     MM EEEEEE MM     MM

// Initialize the Virtual Memory Manager
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool initVMM() {
    // initalize the page directory
    bool rtncde = initPgDir();

    return rtncde;
}