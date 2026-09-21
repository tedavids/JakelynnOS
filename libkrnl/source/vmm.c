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

#include <stdio.h>

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
#define PGDIR_4K                0b100000
#define PGDIR_GLOBAL            0b1000000
#define PGDIR_PAGE_ATTR_TBL     0b10000000

// actual page directory bits
#define DIR_PRESENT             0b1
#define DIR_READ_WRITE          0b10
#define DIR_USER_MEM            0b100
#define DIR_PAGE_WRITE_THRU     0b1000
#define DIR_CACHE_DISABLE       0b10000
#define DIR_4K                  0b10000000
#define DIR_GLOBAL              0b100000000
#define DIR_PAGE_ATTR_TBL       0b1000000000000

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
#define PGDIR_ATTR_MASK             (DIR_PRESENT | DIR_READ_WRITE | DIR_USER_MEM | DIR_PAGE_WRITE_THRU | DIR_CACHE_DISABLE | DIR_4K | DIR_GLOBAL | DIR_PAGE_ATTR_TBL)

#define PGTBL_ATTR_MASK             (VIRT_MEMORY_PRESENT | VIRT_MEMORY_READ_WRITE | VIRT_MEMORY_USER_MEM | VIRT_MEMORY_PAGE_WRITE_THRU | \
                                    VIRT_MEMORY_CACHE_DISABLE | VIRT_MEMORY_4K | VIRT_MEMORY_GLOBAL)

// externals
extern uint32_t page_directory[1024];

// typedefs/structs

typedef uint32_t VirtPageOff_t;
typedef uint8_t  PgDirProp_t[1024];
typedef uint16_t PageTableArray[1024][1024];


// variables 

// page directory Properties

PgDirProp_t     vmmPgDirProp;
PgDirProp_t     *vmmPgDirPropPID = &vmmPgDirProp;
PageDirectory_t *vmmPageDirectoryPID = &page_directory;

// Virtual memory
PageTableArray vmmPageTableProp;
PageTableArray *vmmPageTablePID = &vmmPageTableProp;     // pointer current PID page directory properties
VirtMemInfo_t   VirtMemInfo;

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

// Is the page directory properties Page Attribute Table bit se
// Parameters:  idx - page directory index
// Returns:     true if set, false otherwise
bool pgdirIsPAT(uint32_t idx) {
    // check parameter
    if (idx > 1023) return false;

    return (*vmmPgDirPropPID)[idx] & PGDIR_PAGE_ATTR_TBL;
}

// Is the page directory Page Attribute Table bit se
// Parameters:  idx - page directory index
// Returns:     true if set, false otherwise
bool pgdirIsPATMem(uint32_t idx) {
    // check parameter
    if (idx > 1023) return false;

    return (*vmmPageDirectoryPID)[idx] & DIR_PAGE_ATTR_TBL;
}

// Is the property page directory attribute global
// Parameters:  idx - The property page index
// Returns:     true if is global, false otherwise
bool pgdirIsGlobal(uint32_t idx) {
    return ((*vmmPgDirPropPID)[idx] & PGDIR_GLOBAL);
}

// Is the page directory attribute global
// Parameters:  idx - The page directory index
// Returns:     true if is global, false otherwise
bool pgdirIsGlobalMem(uint32_t idx) {
    return ((*vmmPageDirectoryPID)[idx] & DIR_GLOBAL);
}

// Set the page direcotry property bit to Global
// Parameteers: idx - The index of the property page
// Returns:     true on success, false otherwise
bool pgdiSetGlobal(uint32_t idx) {
    // if it is already set we can't set it agin
    if (pgdirIsGlobal(idx)) return false;
    // set it
    (*vmmPgDirPropPID)[idx] |= PGDIR_GLOBAL;
    return true;
}

// Clear the page directory property bit for global
// Paraters:    idx - The indx of the proper page
// Returns:     true if successful, false otherwise
bool pgdirClearGlobal(uint32_t idx) {
    // if the bit is clear, we can't clear it again
    if (!pgdirIsGlobal(idx)) return false;

    // clear the bit
    (*vmmPgDirPropPID)[idx] = (uint8_t)~PGDIR_GLOBAL;
    return true;
}

// Change the global bit on the page direcory
// Parameters:  idx - The index of the page directory
//              seton - Do we set teh bit on (otherwise off)
//              invalidate - Do we want to invalidate the page
bool pgdirChgGlobalMem(uint32_t idx, bool seton, bool invalidate) {
    // set bit on
    if (seton) {
        // if it is already on we can't set it again
        if (pgdirIsGlobalMem(idx)) return false;
        // set it on
        (*vmmPageDirectoryPID)[idx] |= DIR_GLOBAL;
    } else {
        // clear bit
        // if already clear we can't clear it again
        if (!pgdirIsGlobalMem(idx)) return false;
        // clear it
        (*vmmPageDirectoryPID)[idx] &= (uint8_t)~DIR_GLOBAL;
    }
    // invalidate if necessary
    if (invalidate) {
        invalidatePage(&(*vmmPageDirectoryPID)[idx]);
    }

    return true;
}

// Change the properties to an Attribute mask
// Parameters:  idx - index of property array
// Returns:     a mask of the properties or 0 if errnor
uint32_t pgdirPropToAttr(uint32_t idx) {
    // check index 
    if (idx > 1023) return 0;
    // if it isn't present, you shouldn't be checking
    if (!((*vmmPgDirPropPID)[idx] & PGDIR_PRESENT)) return 0;

    uint32_t mask = DIR_PRESENT;
    if ((*vmmPgDirPropPID)[idx] & PGDIR_READ_WRITE) mask |= DIR_READ_WRITE;
    if ((*vmmPgDirPropPID)[idx] & PGDIR_USER_MEM) mask |= DIR_USER_MEM;
    if ((*vmmPgDirPropPID)[idx] & PGDIR_PAGE_WRITE_THRU) mask |= DIR_PAGE_WRITE_THRU;
    if ((*vmmPgDirPropPID)[idx] & PGDIR_CACHE_DISABLE) mask |= DIR_CACHE_DISABLE;
    if ((*vmmPgDirPropPID)[idx] & PGDIR_4K) mask |= DIR_4K;
    if ((*vmmPgDirPropPID)[idx] & PGDIR_GLOBAL) mask |= DIR_GLOBAL;
    if ((*vmmPgDirPropPID)[idx] & PGDIR_PAGE_ATTR_TBL) mask |= DIR_PAGE_ATTR_TBL;

    return mask;  
}

// Evaluate page directory entry and see if it matches the attributes
// ignores dirty and accesses bits
// Parameters:  idx - the page directories index
// Returns:     True if OK, false otherwise
bool pgdirEvalPde(uint32_t idx) {
    // check parameters
    if (idx > 1023) return false;
    auto dir = ((*vmmPageDirectoryPID)[idx] & PGDIR_ATTR_MASK);
    auto mask = pgdirPropToAttr(idx);
    if (dir != mask) printf("pgdirEvalPde failed on index idx(0x%xl), dir = 0x%xl, mask = 0x%xl\n\r", idx,dir,mask);
    return (dir == mask);
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

    (*vmmPageDirectoryPID)[idx] &=  mask;
    if (invalidate) {
        invalidatePage(&(*vmmPageDirectoryPID)[idx]);
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
        if ((*vmmPageDirectoryPID)[i] & DIR_PRESENT) {
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

// Validate the page directory attributes with the actual page directory
// Parameters:  sync -- Resync attributes from actual page directory
// Returns:     true if in sync, false otherwise, sync should ensure a true return
extern bool pgdirValidatePgDir(bool sync) {
    return pgdirSyncAttr(sync);
}

// Initialize the Page Directory structures
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool pgdirInitPgDir() {
    bool rtncde = true;

    for (uint32_t i = 0; i < 1024; i++) {
        //make sure entry is i n use
        if ((*vmmPageDirectoryPID)[i]) {
            (*vmmPgDirPropPID)[i] = 0; // make sure it's clean
            if ((*vmmPageDirectoryPID)[i] & DIR_PRESENT) (*vmmPgDirPropPID)[i] |= PGDIR_PRESENT;
            if ((*vmmPageDirectoryPID)[i] & DIR_READ_WRITE) (*vmmPgDirPropPID)[i] |= PGDIR_READ_WRITE;
            if ((*vmmPageDirectoryPID)[i] & DIR_USER_MEM) (*vmmPgDirPropPID)[i] |= PGDIR_USER_MEM;   
            if ((*vmmPageDirectoryPID)[i] & DIR_PAGE_WRITE_THRU) (*vmmPgDirPropPID)[i] |= PGDIR_PAGE_WRITE_THRU;
            if ((*vmmPageDirectoryPID)[i] & DIR_CACHE_DISABLE) (*vmmPgDirPropPID)[i] |= PGDIR_CACHE_DISABLE;
            if ((*vmmPageDirectoryPID)[i] & DIR_4K) (*vmmPgDirPropPID)[i] |= PGDIR_4K;
            if ((*vmmPageDirectoryPID)[i] & DIR_GLOBAL) (*vmmPgDirPropPID)[i] |= PGDIR_GLOBAL;
            if ((*vmmPageDirectoryPID)[i] & DIR_PAGE_ATTR_TBL) (*vmmPgDirPropPID)[i] |= PGDIR_PAGE_ATTR_TBL;
        }
    }
    return rtncde;
}

// Externals

// Set the current page directory
// Parameters:  pgdir - A pointer to the page directory
// Returns:     The old page directory
PageDirectory_t *pgdirSetPgDir(PageDirectory_t *dir) {
    auto olddir = vmmPageDirectoryPID;
    vmmPageDirectoryPID = dir;
    return olddir;
}

// VV         VV IIIIII RRRRRR  TTTTTTTT   MM     MM EEEEEE MM     MM
//  VV       VV    II   RR   RR    TT      MMM   MMM EE     MMM   MMM
//   VV     VV     II   RR   RR    TT      MMMM MMMM EEEEE  MMMM MMMM
//    VV   VV      II   RRRRRR     TT      MM  M  MM EE     MM  M  MM
//     VV VV       II   RR  RR     TT      MM     MM EE     MM     MM
//      VVV      IIIIII RR   RR    TT      MM     MM EEEEEE MM     MM

// Internals

// virtual memory externals
// Is the page a kernel page (ring 3)
// Parameters:  virtaddr - The address to check
// Returns:     true if the address is assigned to the kernel, false otherwise
bool vmmIsKrnlMem(VirtAddr_t virtaddr) {
    // get the page table index and page director index
    auto pde = PdeFromAddress(virtaddr);
    auto pte = PteFromAddress(virtaddr);

    return ((*vmmPageTablePID)[pte][pde]  &  (uint16_t)VIRT_MEMORY_KERNEL);
}


// Is the page a user page (ring 0)
// Parameters:  virtaddr - the virtual address to check
// Returns:     true if the kernel memory attribute is not set, false if it is
bool vmmIsUserMem(VirtAddr_t virtaddr) {
    return !vmmIsKrnlMem(virtaddr);
}

// Set the kernel attribute for the address
// The companion function is vmmSetUserMem()
// Parametwers: virtaddr - The virtual address to set to kernel
// Returns:     true if it was set, false if it wasn't (it was already kernel)
bool vmmSetKrnlMem(VirtAddr_t virtaddr) {
    // if it is already kernel mem we can't
    if (vmmIsKrnlMem(virtaddr)) return false;

    // get the page table index and page director index
    auto pde = PdeFromAddress(virtaddr);
    auto pte = PteFromAddress(virtaddr);

    ((*vmmPageTablePID)[pte][pde] |=  (uint16_t)VIRT_MEMORY_KERNEL);
    return true;
}

// Clear the kernel attribute for the address
// The companion function is vmmSetKrnlMem()
// Parametwers: virtaddr - The virtual address to set to User
// Returns:     true if it was set, false if it wasn't (it was already user)
bool vmmSetUserMem(VirtAddr_t virtaddr) {
     // if it is already kernel mem we can't
    if (vmmIsUserMem(virtaddr)) return false;

    // get the page table index and page director index
    auto pde = PdeFromAddress(virtaddr);
    auto pte = PteFromAddress(virtaddr);

    ((*vmmPageTablePID)[pte][pde] &= (uint16_t)~((uint16_t)VIRT_MEMORY_KERNEL));
    return true;
}

// externals

// Get information about the virtual memory
// Parameters:  none
// Returns:     Information on virtual memory
VirtMemInfo_t vmmVirtMemInfo() {
    return VirtMemInfo;
}

// Initialize the Virtual Memory Manager
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool initVMM() {
    // initalize the page directory
    bool rtncde = pgdirInitPgDir();

    // clear Virtual Memory Information
    VirtMemInfo.KernelPages = 0;
    VirtMemInfo.NonSwappablePages = 0;
    VirtMemInfo.ReadOnlyPages = 0;
    VirtMemInfo.SharedPages = 0;
    VirtMemInfo.SwappablePages = 0;
    VirtMemInfo.SwappedPages = 0;
    VirtMemInfo.UsedPages = 0;

    return rtncde;
}