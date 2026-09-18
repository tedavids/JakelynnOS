// Physical Memory Manager (PMM)

#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stdbool.h>
#include <multiboot.h>

// Defines

// number of bits to shift (left or right) to change a page offset to an address or visa versa
#define MEM_PAGE_SHIFT              12
// Number of bytes in a page of memory
#define MEM_PAGE_SIZE               0x1000
// Memory range error
#define MEM_RANGE_ERROR             0xFFFFFFFF
// memory address error
#define MEM_ADDRESS_ERROR           0xFFFFFFFF

// Macros

// typedefs

typedef uint32_t MemAddr_t;            // generic memory address (physical or virtual)
typedef MemAddr_t PhysAddr_t;          // physical memory address
typedef MemAddr_t RsvdAddr_t;          // Reserved address
typedef MemAddr_t VirtAddr_t;          // Virtual Address Type
typedef uint32_t PageOff_t;            // generic page offset
typedef uint32_t pde_t;                // page directory entry index
typedef uint32_t pte_t;                // page table entry index

typedef uint32_t pagetable_array_t[1024][1024]; // page table array

// contains information about physical memory in general
typedef struct {
    uint32_t    numPagesExist;         // number of pages that exist
    uint32_t    numPagesInUse;         // number of pages in use
    uint32_t    numPagesAvail;         // number of page available
    uint32_t    numPagesRsvd;          // number of pages reserved
    uint32_t    numPagesAvailInUse;    // number of pages available and in use
    uint32_t    numPagesRsvdInUse;     // number of page reserved and in use
    uint32_t    numPagesDefect;        // number of pages defective
} PhysMemInfo_t;

// generic memory range
typedef struct {
    RsvdAddr_t    startaddr;        // starting memory addrees
    RsvdAddr_t    endaddr;          // ending address
} MemRange_t;

typedef MemRange_t RsvdMemRange_t;  // reserved memory range
    // generic page offset

// variables
extern pagetable_array_t * const    PAGETABLE;

// functions

// get the number of reserved physical memory ranges
// Parameters:  None
// Returns:     The number of reserved Memory ranges (not counting recoverable ones)
extern uint32_t pmmNumRsvdMemRanges();

// get the physical memory information
// Parameters:  None
// Returns:     The physical memory information
extern PhysMemInfo_t pmmGetPhysMemInfo();

// Get a specific reserved memory range
// Parameters:  idx - The range to retrieve (0 based)
// Returns      The memory range of the index, if error the stating address of the range is MEM_RANGE_ERROR
extern RsvdMemRange_t pmmGetRsvdMemRange(uint32_t idx);

// Get the total amount of physical memory (in bytes)
// Parameters:  None
// Returns:     The number of bytes of physical memory
extern uint32_t pmmMemSize();

// Allocate a physical address
// Parameters:  None
// Returns:     The physical address allocated, or nullptr on failure
extern PhysAddr_t pmmAlloc();

// Free a physical address
// Parameters:  physaddr - The address you got from physAlloc()
// Returns:     true if free worked, false otherwise
extern bool pmmFree(PhysAddr_t physaddr);

// Is the page reserved
// Parameters:  virtaddr - the address you want to know about
// Returns:     true if reserved, false otherwise
extern bool pmmIsPageRsvd(VirtAddr_t virtaddr);

// Allocate a reserved address 
// Parameters:  physaddr - the physical address to allocate
// Returns:     The physical address, or nullptr on failure
extern RsvdAddr_t pmmAllocRsvd(RsvdAddr_t rstdaddr) ;

// Free a reserved address
// Parameters:  rsvdaddr - The reserved returned by physAllocRsvd()
// Returns:     true if successful, false if an error occured
extern bool pmmFreeRsvd(RsvdAddr_t rsvdaddr);

// Allocate a range of reserved memory
// Parameters:  range - The memory range this must match the range provided by physGetRsvdMemRange(x)
// Returns:     The start address of the memory range, or MEM_RANGE_ERROR
extern RsvdAddr_t pmmAllocRsvdRange(RsvdMemRange_t range);

// Free a reserved memory range
// Parameters:  range -- The address range to free
// Returns:     true if successful, false otherwise
extern bool pmmFreeRsvdRange(RsvdMemRange_t range);

// get the page offset from an address (both physical and virtual)
// Parameters:  addr - The address to convert
// Returns:     The page offset for the given address
extern PageOff_t AddressToPageOff(MemAddr_t addr);

// Get an address from a page offset
// Parameters:  pageoff - The page offset to convert
// Returns:     The first address for the page given
extern MemAddr_t PageOffToAddress(PageOff_t pagoff);

// align a page address up, IF it is not on a page boundry
// Parameters:  addr -- the address to align
// Returns:     The aligned address
extern MemAddr_t PageAlignUp(MemAddr_t addr);

// align a page address down, IF it is not on a page boundry
// Parameters:  addr -- the address to align
// Returns:     The aligned address
extern MemAddr_t PageAlignDown(MemAddr_t addr);


// flush the Translation Lookaside Buffer (TLB)
// Parameters:  none
// returns;     None
extern void FlushTLB();

// invalidate an individual page
// Parameters:  virtaddress -- The virtual address of the page to be invalidated
// Returns:     None
void invalidatePage(VirtAddr_t * virtaddr);

// Initialize physical memory structures
// Parameters:  mmap -- Physical memory map
// Returns:     true if successful, false otherwise
bool initPMM(const multiboot_mem_map_info_t * const mmap);

#endif