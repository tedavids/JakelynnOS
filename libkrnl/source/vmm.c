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


#define PGTBL_ATTR_MASK             (VIRT_MEMORY_PRESENT | VIRT_MEMORY_READ_WRITE | VIRT_MEMORY_USER_MEM | VIRT_MEMORY_PAGE_WRITE_THRU | \
                                    VIRT_MEMORY_CACHE_DISABLE | VIRT_MEMORY_4K | VIRT_MEMORY_GLOBAL)



// typedefs/structs

typedef uint32_t VirtPageOff_t;
typedef uint16_t PageTableArray[1024][1024];


// variables 


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
    if (!vmmPdePteValid(pde, pte)) return MEM_ADDRESS_ERROR;

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

// are the pte and pde valid
// Parameters:  pde - the page direcory index
//              pte - the page table index
// Returns:     true if the indexes are valid, false otherwise
bool vmmPdePteValid(pde_t pde, pte_t pte) {
    return ((pde < 1024) && (pte < 1024));
}

// Get information about the virtual memory
// Parameters:  none
// Returns:     Information on virtual memory
VirtMemInfo_t vmmVirtMemInfo() {
    return VirtMemInfo;
}

// Initialize the Virtual Memory Manager
// Parameters:  None
// Returns:     true if successful, false otherwise
bool initVMM() {
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