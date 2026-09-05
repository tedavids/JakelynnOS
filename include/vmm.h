// virtual Memory Manager

#ifndef VMM_H
#define VMM_H

#include <stdbool.h>
#include <stdint.h>

#include <pmm.h>

// structures
struct VirtMemInfo_t {
    uint32_t    UsedPages;
    uint32_t    SharedPages;
    uint32_t    ReadOnlyPages;
    uint32_t    KernelPages;
    uint32_t    SwappablePages;
    uint32_t    SwappedPages;
};

// External variables
extern struct VirtMemInfo_t VirtMemInfo;

// external for testing only
// get the next available kernel address
// Parameters:  None
// returns:     the next available kernel page, or zero if out of memory
uint32_t getNextAvailKernelPage();

// external functions
// get virtual address from a page directory enty and page table entry

// Parameters:  pde - The page direcory entry index
//              pte - The page table entry index

// Returns:     The virtual address of the entries
//              or 0xFFFFFFFF if it fails

extern uint32_t virtAddrFromPdePdt(uint32_t pde, uint32_t pte);

// get if virtual memory is part of the kernel

extern bool isVirtMemKernel(uint32_t virtaddr);

// get if virtual memory page is read only
extern bool isVirtMemReadOnly(uint32_t virtaddr);
// set virtual memory to read only
extern bool setVirtMemReadOnly(uint32_t virtaddr);
// clear virtual memory read only flag
extern bool clearVirtMemReadOnly(uint32_t virtaddr);

// get if virtual memory is swappable
extern bool isVirtMemSwappable(uint32_t virtaddr);
// set virtual memory to swappable
extern bool setVirtMemSwappable(uint32_t virtaddr);
// clear virtual memory swapable
extern bool clearVirtMemSwappable(uint32_t virtaddr);

// get if virtual memory page is swapped out to disk
extern bool isVirtMemSwappedOut(uint32_t virtaddr);
// set virtual memory to swapped out
extern bool setVirtMemSwappedOut(uint32_t virtaddr);
// clear virtual memory swapped out flag
extern bool clearVirtMemSwappedOut(uint32_t virtaddr);

// get if virtual memory is in use
extern bool isVirtMemInUse(uint32_t virtaddr);
// set virtual memory to in use
extern bool setVirtMemInUse(uint32_t virtaddr);
// clear virtual memory in use flag
extern bool clearVirtMemInUse(uint32_t virtaddr);

// get if it is kernel memory
extern bool isVirtKrnlMem(uint32_t virtaddr);
// set it as kernel memory
extern bool setVirtKrnlMem(uint32_t virtaddr);
// clear kernel memory flag
extern bool clearVirtKrnlMem(uint32_t virtaddr);

// get if virtual memory is shared
// Parameters:  virtaddr -- the address to check
// Returns:     true if shared, false if not or there is an issue
extern bool isVirtMemShared(uint32_t virtaddr);
// set virtual memory to shared
extern bool setVirtMemShared(uint32_t virtaddr);
// clear shared bit
extern bool clearVirtMemShared(uint32_t virtaddr);

// alloc kernel virtual memory anonomously
// Parameters:  invalidatepage -- invalidate the page
// Returns:     true if successful, false otherise
extern uint32_t * allocAnonKrnlMem(bool invalidatepage);

// dealloc any unused anonomous kernel memory
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool deallocAnonKrnlMem();

// allocate kernel virtual memory
// Parameters:  virtaddr -- the address you wish allocated
//              invalidatepage -- do you want to invalidate the page
// Returns:     the address of the allocation, or nullptr if unsuccessful
extern uint32_t* allocVirtKrnlMem(uint32_t virtaddr, bool invalidatepage);

// deallocate kernel virtual memory
// Parameters:  virtaddr -- the address you wish allocated
//              invalidatepage -- do you want to invalidate the page
// Returns:     true if succesful, false otherwise
extern bool deallocVirtKrnlMem(uint32_t virtaddr, bool invalidatepage);

// allocate virtual memory
// Parameters:  virtaddr -- the address you wish allocated
//              invalidatepage -- do you want to invalidate the page
// Returns:     the address of the allocation, or nullptr if unsuccessful
extern uint32_t * allocVirtMem(uint32_t virtaddr, bool invalidatepage);

// deallocate virtual memory
// Parameters:  virtaddr -- the address you with to deallocate
//                          it must have been allocated by allocVirtMem
//              invalidatepage -- do you want to invalidate the page
// Returns:     true if successful, false otherwise
extern bool deallocVirtMem(uint32_t virtaddr, bool invalidatepage);

// allocate a block of virtual memory
// Parameters:  startaddr -- the starting address you want to allocate
//              endaddr -- the ending address you want to allocate
//              *lastallocated -- the last page allocated on error, if 0 none were done

// Returns:     the address of the allocation, or nullptr if unsuccessful
extern uint32_t* allocVirtMemBlock(uint32_t startaddr, uint32_t endaddr, uint32_t *lastallocated);

// deallocates a block of virtual memory
// Parameters:  startaddr - The start address of the deallocation
//              endaddr -- the end address of the deallocation
//              lastdealloc -- the last address actually deallocated

// returns:     true if successful, false otherwise
extern bool deallocVirtMemBlock(uint32_t startaddr, uint32_t endaddr, uint32_t *lastdealloc);

// allocate virtual memory
// Parameters:  virtaddr -- the address you wish multi allocated, it must already have been allocated by allocVirtMem
// Returns:     The address of the shared memory, or nullptr if it fails
extern uint32_t* allocSharedVirtMem(uint32_t addrtoshare, uint32_t virtaddr);

// deallocate virtual memory
// Parameters:  virtaddr -- the address you with to deallocate
//                          it must have been allocated by allocSharedVirtMem, unless it was the first allocation
// Returns:     true if successful, false otherwise

// Note: The user is responsible to keep track of the number of times it's been shared
extern bool deallocSharedVirtMem(uint32_t virtaddr, bool lastaddr);

// initialize Virtual Memory Manager
bool initVMM();

#endif