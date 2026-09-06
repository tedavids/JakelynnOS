// Physical Memory Management

#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stdbool.h>

#include <multiboot.h>

// macros 
// Round UP to the next page boundary
#define PAGE_ALIGN_UP(addr)   (((addr) + 0x1000 - 1) & 0xFFFFF000)

// Round DOWN to the previous page boundary
#define PAGE_ALIGN_DOWN(addr) ((addr) & 0xFFFFF000)

// structures
struct PhysMemInfo_t {
    uint32_t    PagesExist;
    uint32_t    PagesInUse;
    uint32_t    PagesAvail;
    uint32_t    PagesReserved;
};

typedef uint32_t pte_t;                 // page table entry
typedef uint32_t pde_t;                 // page directory entry
typedef uint32_t page_table_t [1024];    // individual page table


// external variables
extern uint32_t                 page_directory[1024];
extern struct PhysMemInfo_t     PhysMemInfo;
extern uint32_t * const         PAGETABLE;
// flush the transaction lookaside buffer
extern void     FlushTLB();

// invalidate an individual page
// Parameters:  virtaddress -- The virtual address of the page to be invalidated
// Returns:     None
extern void     invalidatePage(void * virtaddress);


// get the physical address of the pde/pdt entry
// Parameters:  address -- a pointer to the address the function returns
//              pde -- the page directory entry (index)
//              pte -- the page table entry (index)

// returns:     true if found, false otherwise
//              if true, address  containts the address, otherwise it is set to a null pointer
bool physAddrFromPdePdt(uint32_t *address, uint32_t pde, uint32_t pte);

// get PDE physical address
// Parameters:  address -- a pointer to where you want the physical address
//              pde -- the index in the page direcory
// Returns:     true if there is a phsical address, false otherwise
//              if true address is the physical address, otherwise address is a null pointer
bool physAddrOfPDE(uint32_t *address, pde_t pde);

// get page from address
uint32_t AddressToPage(uint32_t address);

// get the page directory entry from a virtual address
pde_t getPDEFromAddress(uint32_t virtaddr);

// get the page table enry from a virtual address
pde_t getPTEFromAddress(uint32_t virtaddr);

// get the page table physical address from a PDE,PDT
// Parameters:  pde - The page direcory entry
//              pte - The table table entry
// returns:     The physical address of the combination, or 0xFFFFFFF if failed

extern uint32_t getPageTablePhysAddress(uint32_t pde, uint32_t pte);

// get the virtual address of a particular page table
// Parameters:  pde -- The page directory entry of the table
// Returns:     The virtual address of the page table
extern uint32_t getPageTableVirtAddress(pde_t pde, pte_t pte);

// get if physical page exists
extern bool physMemExists(uint32_t phypageoffset);
// get if physical memory page is available (not reserved)
extern bool physMemAvail(uint32_t phypageoffset);
// get if physical memory page is reserved
extern bool physMemReserved(uint32_t phypageoffset);
// get if physical memory is in a page table
// also returns false if physical memory doesn't exist
extern bool physMemInUse(uint32_t phypageoffset);


// allocate a physical memory page

// parameters:  physpage the physical page that is allocated

// Returns:     true if page allocated, false otherwise
extern bool allocPhysMem(uint32_t *physpage);

// allocate a physical page more than once
// note:  It is up to the caller to make sure it isn't deallocated prematurely
extern bool multiAllocPhysMem(uint32_t physpage);

// deallocate a phyical page

// Parameters:  physpage the phiscal page to remove from a page table

// Returns:     true if removed, false if not (it was multiple allocated, or doesnt' exist)

extern bool deAllocPhysMem(uint32_t physpage);

// Initialize the page directory
// this must happen after we do multiboot, because it clears page 0

// Parameters:  None

// Returns:     true if successful, false otherwise

extern bool initPageDirectory();

// process the multiboot memory map

// external ONLY for testing

// Parameters:  mmap -- the multiboot memory map
// Returns:     true if successful

extern bool processMultibootMemMap(const struct multiboot_mem_map_info_t * const mmap);

// set in use memory flags
// external ONLY for testing

// Parameters:  None
// Returns:     the number of errors it found (should be 0)

extern uint32_t setInUsePhysicalMemory();

// set the tail for free physical memory

// initialize memory structures

// Parameters:  None

// Returns:     true if structures initialized, false otherwise
extern bool initPMM(const struct multiboot_memory_info_t * const meminfo,  const struct multiboot_mem_map_info_t * const mmap);
#endif