// Virtual Memory Manager

#ifndef _VMM_H
#define _VMM_H

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

#include <stddef.h>

#include <pmm.h>

// defines

#define VIRT_MEMORY_ACCESSED    0b100000
#define VIRT_MEMOYR_DIRTY       0b1000000

// typedefs
typedef struct {
    uint32_t    UsedPages;
    uint32_t    SharedPages;
    uint32_t    ReadOnlyPages;
    uint32_t    KernelPages;
    uint32_t    SwappablePages;
    uint32_t    NonSwappablePages;
    uint32_t    SwappedPages;
} VirtMemInfo_t;




// get an address from a page directory entry and page table entry
// Parameters:  pde - the page directory entry
//              pte - the page table entry
// Returns:     The address of the page matching the entries
extern MemAddr_t AddrFromPdePte(pde_t pde, pte_t pte);

// get Page directory entry index from an address
// Parameters:  address - The address to get the pde from
// Returns:     the pde for the address
extern pde_t PdeFromAddress(MemAddr_t address);

// get Page table entry index from an address
// Parameters:  address - The address to get the pde from
// Returns:     the pde for the address
extern pte_t PteFromAddress(MemAddr_t address);

// page directory externals

// Validate the page directory attributes with the actual page directory
// Parameters:  sync -- Resync attributes from actual page directory
// Returns:     true if in sync, false otherwise, sync should ensure a true return
extern bool pgdirValidatePgDir(bool sync);

// Initialize the Page Directory structures
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool initPgDir();

// virtual memory externals
// Is the page a kernel page (ring 3)
// Parameters:  virtaddr - The address to check
// Returns:     true if the address is assigned to the kernel, false otherwise
extern bool vmmIsKrnlMem(VirtAddr_t virtaddr);

// Set the kernel attribute for the address
// The companion function is vmmSetUserMem()
// Parametwers: virtaddr - The virtual address to set to kernel
// Returns:     true if it was set, false if it wasn't (it was already kernel)
extern bool vmmSetKrnlMem(VirtAddr_t virtaddr);

// Is the page a user page (ring 0)
// Parameters:  virtaddr - the virtual address to check
// Returns:     true if the kernel memory attribute is not set, false if it is
extern bool vmmIsUserMem(VirtAddr_t vrtaddr);

// Clear the kernel attribute for the address
// The companion function is vmmSetKrnlMem()
// Parametwers: virtaddr - The virtual address to set to User
// Returns:     true if it was set, false if it wasn't (it was already user)
extern bool vmmSetUserMem(VirtAddr_t virtaddr);

// is the memory read only
// Parameters:  virtaddr - the virtual address to check
// Returns:     true if the memory is read only, false if it is read/write
extern bool vmmMemReadOnly(VirtAddr_t virtaddr);

// is the memory read/write
// Parameters:  virtaddr - the adress to check
// Returns:     true if the memory is read/write, false if read only
extern bool vmmMemReadWrite(VirtAddr_t virtaddr);

// Set memory to read only, this will change the attribute table also if necessary
// Parameters:  virtaddr - the address of the page to change
//              invalidate - invalidate page (if aren't about to FlushTLB())
// Returns:     true if successful, false if not (should always succeed)
extern bool vmmMemSetReadOnly(VirtAddr_t virtaddr, bool invalidate);

// Set the memory to read/write, this will also change the attribute table if necessary
// Parameters:  virtaddr - the address of the page to change
//              invalidate - Invalidate page (if you aren't about to FlushTLB())
// Returns:     true if successful, false if not (this shoudl always succeed)
extern bool vmmMemSetReadWrite(VirtAddr_t virtaddr, bool invalidate);

// is the memory swappable
// Parameters:  virtaddr - the address of the page to check
// Returns:     true if the swappable flag is set, false otherwise
extern bool vmmIsSwappable(VirtAddr_t virtaddr);

// Set the attribute to shared
// Parameters:  virtaddr - The address of the page to share
// Returns:     true if successful, false if already swappable
extern bool vmmSetSwappable(VirtAddr_t virtaddr);

// Clear the shared attribute
// Parameters:  virtaddr - The address of the page
// Returns:     true if successful, false if not already swappable
extern bool vmmClearSwappable(VirtAddr_t virtaddr);

// is the memory swapped out
// Parameters:  virtaddr - the address of the page to check
// Returns:     true if the swapped out flag is set, false otherwise
extern bool vmmIsSwappedOut(VirtAddr_t virtaddr);

// Set the attribute to swapped out
// Parameters:  virtaddr - The address of the page to swap out
// Returns:     true if successful, false if already swapped out
extern bool vmmSetSwappedOut(VirtAddr_t virtaddr);

// Clear the swapped out attribute
// Parameters:  virtaddr - The address of the page
// Returns:     true if successful, false if not already swapped out
extern bool vmmClearSwappedOut(VirtAddr_t virtaddr);

// is the memory shared
// Parameters:  virtaddr - the address of the page to check
// Returns:     true if the shared flag is set, false otherwise
extern bool vmmIsShared(VirtAddr_t virtaddr);

// Set the attribute to shared
// Parameters:  virtaddr - The address of the page to share
// Returns:     true if successful, false if already shared
extern bool vmmSetShared(VirtAddr_t virtaddr);

// Clear the shared attribute
// Parameters:  virtaddr - The address of the page
// Returns:     true if successful, false if not already shared
extern bool vmmClearShared(VirtAddr_t virtaddr);

// Is the page in use
// Parameters:  virtaddr - the address of the page to check
// Returns:     true if in use, false otherwise
extern bool vmmInUse(VirtAddr_t virtaddr);

// Set the page in use
// Parameters:  virtaddr - the address of the page to set
//              invalidate - invalidate the page (if you don't intend to do a FlushTLB)
// Return:      true if successful, false if it is already in use
extern bool vmmSetInUse(VirtAddr_t virtaddr, bool invalidate);

// Clear the page in use flag
// Parameters:  virtaddr - the address of the page to clear
//              invalidate - invalidate the page (if you don't intend to do a FlushTLB)'
extern bool vmmClearInUse(VirtAddr_t virtaddr, bool invalidate);

// Allocate memory from the top of the kernel address range
// Parametrs:   invalidate -- invalidate the page (if you dont intend to do a FlushTLB)
// Returns:     true if successful, false otherwise (should always return true)
extern VirtAddr_t vmmKAllocTop(bool invalidate);

// Allocate memory from the bottom of the kernel address range
// Parametrs:   invalidate -- invalidate the page (if you dont intend to do a FlushTLB)
// Returns:     true if successful, false otherwise (should always return true)
extern VirtAddr_t vmmKAllocBottom(bool invalidate);

// Free kernel memory
// Parameters:  vitaddr - the address of the page to free, shoudl have come from 
//                          vmmKKllocTop or vmmKAllocBottom
//              invalidate - invalidate the page (if you don't intend to do a FlushTLB)
extern bool vmmKFree(VirtAddr_t virtaddr, bool invalidate);

// Allocate memory from the top of the user address range
// Parametrs:   invalidate -- invalidate the page (if you dont intend to do a FlushTLB)
// Returns:     true if successful, false otherwise (should always return true)
extern VirtAddr_t vmmUAllocTop(bool invalidate);

// Allocate memory from the bottom of the user address range
// Parametrs:   invalidate -- invalidate the page (if you dont intend to do a FlushTLB)
// Returns:     true if successful, false otherwise (should always return true)
extern VirtAddr_t vmmUAllocBottom(bool invalidate);

// Free user memory
// Parameters:  vitaddr - the address of the page to free, shoudl have come from 
//                          vmmUKllocTop or vmmuAllocBottom
//              invalidate - invalidate the page (if you don't intend to do a FlushTLB)
extern bool vmmUFree(VirtAddr_t virtaddr, bool invalidate);

// get a range of pages from the top of the kernel range
// Parameters:  numpages - the number of pages you need
// Returns:     The range, if the range.startaddr = MEM_RANGE_ERROR the addresses are not available
extern MemRange_t getAvailKrnlPageRangeTop(uint32_t numpages);

// get a range of pages from the bottom of the kernel range
// Parameters:  numpages - the number of pages you need
// Returns:     The range, if the range.startaddr = MEM_RANGE_ERROR the addresses are not available
extern MemRange_t getAvailKrnlPageRangeBottom(uint32_t numpages);

// Allocate memory from the top of the kernel address range
// Parameters:  size - the number of byte
//              invalidate -- invalidate the range (if you dont intend to do a FlushTLB)
// Returns:     The range, if the range.startaddr = MEM_RANGE_ERROR the addresses are not available
MemRange_t vmmKAllocTopBlock(size_t size, bool invalidate);

//Get a number of bytes from the top of the kernel range
// Parameters:  size - the number of byte
//              invalidate -- invalidate the range (if you dont intend to do a FlushTLB)
// Returns:     The range, if the range.startaddr = MEM_RANGE_ERROR the addresses are not available
MemRange_t vmmKAllocBottomBlock(size_t size, bool invalidate);

// Get a number of bytes fromt the bottom of the kernel range
// Parameters:  virtrange - The range to free, this should have come from 
//                              vmmKAllocTopBlock or vmmKAllocBottomBlock
// Returns:     true if the deallocation worked
extern bool vmmKFreeBlock(MemRange_t virtrange, bool  invalidate) ;

// Allocate memory from the top of the user address range
// Parametrs:   size - the number of bytes to allocate
//              invalidate -- invalidate the page (if you dont intend to do a FlushTLB)
// Returns:     the memory range if successful, if range.first == MEM_RANGE_ERROR it failed
extern VirtAddr_t vmmUAllocTopBlock(size_t size,bool invalidate);

// Allocate memory from the bottom of the user address range
// Parametrs:   size - the number of bytes to allocate
//              invalidate -- invalidate the page (if you dont intend to do a FlushTLB)
// Returns:     the memory range if successful, if range.first == MEM_RANGE_ERROR it failed
extern VirtAddr_t vmmUAllocBottomBlock(size_t size,bool invalidate);

// Free user memory
// Parameters:  virtrange - the address range you wish to free, The range shoudl have come from 
//                          vmmUKllocTopBlock or vmmUAllocBottomBlock
//              invalidate - invalidate the page (if you don't intend to do a FlushTLB)
extern bool vmmUFreeBlock(MemRange_t virtrange, bool invalidate);

// Initialize the Virtual Memory Manager
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool initVMM();
#endif
