// the Physical Memory Manager

#include <registers.h>
#include <stdio.h>
#include <string.h>

#include <pmm.h>

// constants
#define PHYSICAL_MEMORY_EXISTS      0b00000001
#define PHYSICAL_MEMORY_AVAILABLE   0b00000010
#define PHYSICAL_MEMORY_IN_USE      0b00000100
// start of page table
#define MEM_PAGE_TABLE_START        0xFFC00000
#define MEM_PAGE_OFFSET_ERROR       0xFFFFFFFF

extern uint32_t PAGETABLE000[1024];
// typedefs
typedef PageOff_t PhysPageOff_t;        // the physical Page offset

// variables 
uint8_t PhysMemPageSts[0x100000];       // the array that tracks physical memory attributes

PhysMemInfo_t   PhysMemInfo;            // track counts of memory for reporting

uint32_t numRsvdMemRanges;              // the number of reserved Memory ranges

PhysPageOff_t firstPhysPageOff;         // the first physical page offset (> 0)
PhysPageOff_t lastPhysPageOff;          // the last physical page offset
PhysPageOff_t nextPhysPageOff;          // the next available page offset

// the page table 
pagetable_array_t * const    PAGETABLE = (pagetable_array_t *) MEM_PAGE_TABLE_START;

extern uint32_t page_directory[1024];
// functions

// get page from address
// Parameters:  address - the address to get the offset from
// Returns:     The page offset of the address
PageOff_t AddressToPageOff(MemAddr_t address) {
    return (address >> MEM_PAGE_SHIFT);
}

// get the first address from a page offset
// Parameters:  pageoff - the page offset to get the address for
// Returns:     The first memory address on the page
MemAddr_t PageOffToAddress(PageOff_t pageoff) {
    return (pageoff << MEM_PAGE_SHIFT);
} 

// helper functions

// does physical memory exist
// Parameters:  pageoff - physical page offset
// Returns:     true if memory exists, false otherwise
bool pmmMemExists(PhysPageOff_t pageoff) {
    return (PhysMemPageSts[pageoff] & PHYSICAL_MEMORY_EXISTS);
}

// is physical memory available
// Parameters:  pageoff -- the physical page offset
// Returns:     true if the memory is available, false otherwise
bool pmmMemAvail(PhysPageOff_t pageoff) {
    // memory that does not exists is not available
    if (!pmmMemExists(pageoff)) return false;
    
    return (PhysMemPageSts[pageoff] & PHYSICAL_MEMORY_AVAILABLE);
}

// is the physical memory reserved
// Parameters:  pageoff -- the physical page offset
// Return:      True if the memory is reserved, false otherwise
bool pmmMemRsvd(PhysPageOff_t pageoff) {
    // memory that does not exist, can't be reserved
    if (!pmmMemExists(pageoff)) return false;

    return  !pmmMemAvail(pageoff);
}

// is physical memory in use
// Parameters:  pageoff -- the physical page offset
// Returns:     true if memory is reserved, false otherwise
bool pmmMemInUse(PhysPageOff_t pageoff) {
    // if memory does not exist, it can't be in use
    if (!pmmMemExists(pageoff)) return false;

    return (PhysMemPageSts[pageoff] & PHYSICAL_MEMORY_IN_USE);
}

// set physical memory in use
// Parameters:  pageoff -- The physical page offset
// Returns:     true if the set works, false otherwise
bool pmmSetInUse(PhysPageOff_t pageoff) {
    // if the page doesn't exist it can't be set
    if (!pmmMemExists(pageoff)) return false;
    // shared memory will be handled by the VMM, so it can just be marked used once
    if (pmmMemInUse(pageoff)) return false;

    // set it 
    PhysMemPageSts[pageoff] |= PHYSICAL_MEMORY_IN_USE;
    return true;
}

// clear physical memory in use
// Parameters:  pageoff -- The physical page offset
// Returns:     true if cleared, false otherwise
bool pmmClearInUse(PhysPageOff_t pageoff) {
    // if it doesn't exist, it cant be cleared
    if (!pmmMemExists(pageoff)) return false;
    // if it isn't set it cant be cleared
    if (!pmmMemInUse(pageoff)) return false;

    // clear it. xor works becaue we know it is in use
    PhysMemPageSts[pageoff] ^= PHYSICAL_MEMORY_IN_USE;
    return true;
}

// get next available page offset
// Parameters:  None
// Returns:     the physical page offset or MEM_PAGE_OFFSET_ERROR if an error
PhysPageOff_t physGetNextAvailPageOff() {
    // start at next available, hopefully it is available
    while (nextPhysPageOff <= lastPhysPageOff) {
        // is it available
        if (pmmMemAvail(nextPhysPageOff)) {
            // is memory available, is it in use?
            if (!pmmMemInUse(nextPhysPageOff)) {
                // if it overruns it will just skip this loop and fall into the next one
                return nextPhysPageOff++;
            }
        }
        nextPhysPageOff++;
    }
    // it wasn't from next to last, so check from start
    nextPhysPageOff = firstPhysPageOff;
    while (nextPhysPageOff <= lastPhysPageOff) {
        // is memory available
        if (pmmMemAvail(nextPhysPageOff)) {
            // is the memory in use
            if (!pmmMemInUse(nextPhysPageOff)) {
                // we found it
                return nextPhysPageOff++;
            }
        }
    }

    // if we get here we have no more physical memory
    // reset next to first in case swap reclaims memory by the next time
    nextPhysPageOff = firstPhysPageOff;
    return MEM_PAGE_OFFSET_ERROR;
}

// Allocate an available address
// Parameters:  None
// Returns:     the physical page offset or MEM_PAGE_OFFSET_ERROR if memory isn't available
PhysPageOff_t physAllocAvailPage() {
    PhysPageOff_t offset =  physGetNextAvailPageOff();
    // if we have an error don't allocate it
    if (offset == MEM_PAGE_OFFSET_ERROR) return offset;

    // set it in use
    if (pmmSetInUse(offset)) return offset;

    // we have an error, we shouldn't get here
    return MEM_PAGE_OFFSET_ERROR;
}

// Process the mutiboot memory map
// Parameters:  mmap - the multiboot physical memory map
// Returns:     true if successful, false otherwise
bool physProcessMultibootMemMap(const multiboot_mem_map_info_t * const mmap) {
    // initialize physical memory
    memset(PhysMemPageSts,0,sizeof(PhysMemPageSts));

    // make sure we have a memory map
    if (mmap->count == 0) return false;
    
    
    // set head  of our circular available buffer
    uint32_t firstavail = 0;
    while (firstavail < mmap->count) {
        if (mmap->region[firstavail].memtype == 1) break;
    }
    // no available memory?
    if (firstavail > mmap->count) {
        print("No available memory!\n\r");
        return false;
    }

    // get our first available memory
    firstPhysPageOff = 0xFFFFF;

    // last physical offset
    lastPhysPageOff = 0;

    // type of memory
    // last available physical offset
    //lastPhysOffset = (uint32_t)(mmap->region[lastavail].endaddr >> 12);
    // process multiboot memory map
    for (uint32_t i = 0; i < mmap->count; i++) {
        // get starting and ending pages
        uint32_t startaddr;
        uint32_t endaddr;
        switch (mmap->region[i].memtype) {
            // available
            case 1: 
                startaddr = PageAlignUp(((uint32_t)mmap->region[i].baseaddr));
                endaddr = PageAlignDown((uint32_t)mmap->region[i].endaddr);
                if (AddressToPageOff(startaddr) < firstPhysPageOff) firstPhysPageOff = AddressToPageOff(startaddr);
                if (AddressToPageOff(endaddr) > lastPhysPageOff) lastPhysPageOff = AddressToPageOff(endaddr);
                PhysMemInfo.numPagesAvail += AddressToPageOff(endaddr) - AddressToPageOff(startaddr); 
                break;
            // reserved
            case 2:
                startaddr = PageAlignDown(((uint32_t)mmap->region[i].baseaddr));
                endaddr = PageAlignUp((uint32_t)mmap->region[i].endaddr);
                numRsvdMemRanges++;
                break;
            // ACPI reclaimable (treat as type 1)
            case 3:
                startaddr = PageAlignUp(((uint32_t)mmap->region[i].baseaddr));
                endaddr = PageAlignDown((uint32_t)mmap->region[i].endaddr);
                if (AddressToPageOff(startaddr) < firstPhysPageOff) firstPhysPageOff = AddressToPageOff(startaddr);
                if (AddressToPageOff(endaddr) > lastPhysPageOff) lastPhysPageOff = AddressToPageOff(endaddr);
                break;
            // defective (skip)
            case 4:
                startaddr = 9999;
                endaddr = 0;
                PhysMemInfo.numPagesDefect += AddressToPageOff(endaddr) - AddressToPageOff(startaddr);
                break;
            // we don't know (skip it)
            default:
                startaddr = 9999;
                endaddr = 0;
                break;
        }

        // set available memory and if it exists
        for (uint32_t page = AddressToPageOff(startaddr);
            page <= AddressToPageOff(endaddr); page++) {
            // get page offset
            PhysMemPageSts[page] |= PHYSICAL_MEMORY_EXISTS;   // memory exists
            PhysMemInfo.numPagesExist++;

            // is it available to us or is it reserved, if it is reserved, it is also read only
            switch (mmap->region[i].memtype) {
                // available
                case 1: // available
                case 3: // reclaimable
                    PhysMemPageSts[page] |= PHYSICAL_MEMORY_AVAILABLE;
                    break;
                // reserved
                case 2:
                    PhysMemPageSts[page] &= (uint8_t)~(PHYSICAL_MEMORY_AVAILABLE);
                    break;
                // defective
                case 4:
                    break;
                // ignore others
                default:
                    break;
            }
        }
    }

    return true;
}

// Process the video memory
// Parameters:  None
// Returns:     true if successful, false otherwise
// Notes:       Values are hard coded 
bool physProcessVideoMemory() {
    // video memory shows as reserved (not available)
    // i is the offset (not the address)
    for (uint32_t i = 0xA8; i <= 0xBF; i++) {
        PhysMemPageSts[i] |= PHYSICAL_MEMORY_EXISTS;
    }

    return true;
}

// Clear page tables we don't need
// Parameters:  None
// Returns:     true if successful, false otherwise
bool clearUnneededPTEs() {

   // remove mappings to memory that doesn't exist
    for (pde_t pde = 0; pde < 1024; pde++) {
        // make sure it's an active page
        if (page_directory[pde]) {
            // ok now walk the page table
            for (pte_t pte = 0; pte < 1024; pte++) {
                // does it have an entry (rule is when I disable a page table entry it is set to 0)
                if ((*PAGETABLE)[pde][pte]) {
                    auto page = AddressToPageOff((*PAGETABLE)[pde][pte]);
                    // does page exist
                    if (!pmmMemExists(page)) {
                        // if not clear it
                        (*PAGETABLE)[pde][pte] = 0;
                    }
                }
            }
        }
    }

    // we don't need page 1 anymore
    page_directory[1] = 0;

    // we shouldn't need anything on page 1 to 766 anymore
    for (pde_t pde = 2; pde < 767; pde++) {
        if (page_directory[pde]) {
            for (pte_t pte = 0; pte < 1024; pte++) {
                (*PAGETABLE)[pde][pte] = 0;
            }
        }
    }

    return true;
}

// initialize the page tables
// Parameters:  None
// Returns:     true
bool initPageTables() {

    // set up page table 0
    // set pte 0
    PAGETABLE000[0] = 0x3;      // page 0, read/write, present
    //put it in the page page_directory
    page_directory[0] = (uint32_t) &PAGETABLE000 - 0xC0000000 + 0x3; // read/write, present


    bool rtncde = clearUnneededPTEs();

    // flush TLB
    FlushTLB();

    return rtncde;
}

uint32_t pmmSetInUsePhysicalMemory() {
    // no parameters to check

    uint32_t numerrors = 0;

    for (pde_t pde = 0; pde < 1024; pde++) {
        if (page_directory[pde]) {
            // make sure we have a page table
            for (pte_t pte = 0; pte < 1024; pte++) {
                // if its non zero we have an entry
                if ((*PAGETABLE)[pde][pte]) {
                    // get the offset
                    PhysPageOff_t pageoff = (*PAGETABLE)[pde][pte] >> MEM_PAGE_SHIFT;
                    if (pmmMemExists(pageoff)) {
                        PhysMemPageSts[pageoff] |= PHYSICAL_MEMORY_IN_USE;
                    } else {
                        // exclude video memory, it isn't reportd
                        if ((pageoff < 0xA0) || 
                            (pageoff > 0xA7)) {
                            // remove the offending mapping
                            (*PAGETABLE)[pde][pte] = 0;
                            numerrors++;
                        }
                    } 
                }
            }
        }
    }

    // if errors flush the TLB
    if (numerrors) {
        FlushTLB();
    }
    
    return numerrors;
}

// validate that memory range provided is a valid memory range
// Parameters:  range -- memory range to check
// Returns:     true if valid reserved memory range
bool physIsRsvdMemRange(RsvdMemRange_t range) {

    // check ranges
    for (uint32_t i = 0; i < multiboot_info.mmap.count; i++) {
        if (((RsvdAddr_t) multiboot_info.mmap.region[i].baseaddr == range.startaddr) &&
            ((RsvdAddr_t) multiboot_info.mmap.region[i].endaddr == range.endaddr)) {
                return (multiboot_info.mmap.region[i].memtype == 2);
            }
    }

    // if we get here it's not valid
    return false;
}

// get the number of reserved physical memory ranges
// Parameters:  None
// Returns:     The number of reserved Memory ranges (not counting recoverable ones)
extern uint32_t pmmNumRsvdMemRanges() {
    return numRsvdMemRanges;
}

// Get a specific reserved memory range
// Parameters:  idx - The range to retrieve (0 based)
// Returns      The memory range of the index, if error the stating address of the range is MEM_RANGE_ERROR
extern RsvdMemRange_t pmmGetRsvdMemRange(uint32_t idx) {
    // check parameters
    if (!(idx < numRsvdMemRanges)) {
        return (RsvdMemRange_t) {MEM_RANGE_ERROR,0};
    } 

    uint32_t rangenum = 0;
    for (uint32_t i = 0; i < multiboot_info.mmap.count; i++) {
        // is it resrved?
        if (multiboot_info.mmap.region[i].memtype == 2) {
            // is it our range?
            if (rangenum == idx) {
                return (RsvdMemRange_t){(RsvdAddr_t)multiboot_info.mmap.region[i].baseaddr,(RsvdAddr_t)multiboot_info.mmap.region[i].endaddr};
            }
            rangenum++;
        }
    }
    // we shouldn't get her

    return (RsvdMemRange_t) {MEM_RANGE_ERROR,0};
}
// get the physical memory information
// Parameters:  None
// Returns:     The physical memory information
PhysMemInfo_t pmmGetPhysMemInfo() {
    return PhysMemInfo;
}

// Get the total amount of physical memory (in bytes)
// Parameters:  None
// Returns:     The number of bytes of physical memory
extern uint32_t pmmMemSize() {
    return PhysMemInfo.numPagesAvail * MEM_PAGE_SIZE;
}

// Allocate a physical address
// Parameters:  None
// Returns:     The physical address allocated, or nullptr on failure
PhysAddr_t pmmAlloc() {
    auto physpage = physGetNextAvailPageOff();
    // did we get a page
    if (MEM_ADDRESS_ERROR == physpage) return (PhysAddr_t) 0;

    // we have a page, is it available
    if (!pmmMemAvail(physpage)) return (PhysAddr_t) 0;

    // is it in use
    if (pmmMemInUse(physpage)) return (PhysAddr_t) 0;

    if (!pmmSetInUse(physpage)) return (PhysAddr_t) 0;

    // change to address and return it
    return PageOffToAddress(physpage);
}

// Free a physical address
// Parameters:  physaddr - The address you got from physAlloc()
// Returns:     true if free worked, false otherwise
bool pmmFree(PhysAddr_t physaddr) {
    // get the page
    auto physpage = AddressToPageOff(physaddr);
    // is it a valid address
    if (!physpage) return false;

    // is it in use
    if (!pmmMemInUse(physpage)) return false;

    // is it available memory
    if (!pmmMemAvail(physpage)) return false;

    // OK, deallocate it
    if (!pmmClearInUse(physpage)) return false;

    return true;
}

// Is the page reserved
// Parameters:  virtaddr - the address you want to know about
// Returns:     true if reserved, false otherwise
extern bool pmmIsPageRsvd(VirtAddr_t virtaddr) {
    //get the offset
    auto physoffset = AddressToPageOff(virtaddr);
    // does it exist?
    if (!pmmMemExists(physoffset)) return false;

    return pmmMemRsvd(physoffset);
}

// Allocate a reserved address 
// Parameters:  physaddr - the physical address to allocate
// Returns:     The physical address, or nullptr on failure
RsvdAddr_t pmmAllocRsvd(RsvdAddr_t rstdaddr) {
    auto physpage = AddressToPageOff(rstdaddr);
    // did we get a page
    if (MEM_ADDRESS_ERROR == physpage) return (PhysAddr_t) 0;

    // we have a page, is it reserved
    if (!pmmMemRsvd(physpage)) return (PhysAddr_t) 0;

    // is it in use
    if (pmmMemInUse(physpage)) return (PhysAddr_t) 0;

    if (!pmmSetInUse(physpage)) return (PhysAddr_t) 0;

    // change to address and return it
    return PageOffToAddress(physpage);
}

// Free a reserved address
// Parameters:  rsvdaddr - The reserved returned by physAllocRsvd()
// Returns:     true if successful, false if an error occured
bool pmmFreeRsvd(RsvdAddr_t rsvdaddr) {
   // get the page
    auto physpage = AddressToPageOff(rsvdaddr);
    // is it a valid address
    if (!physpage) return false;

    // is it in use
    if (!pmmMemInUse(physpage)) return false;

    // is it reserved memory
    if (!pmmMemRsvd(physpage)) return false;

    // OK, deallocate it
    if (!pmmClearInUse(physpage)) return false;

    return true;   
}

// Allocate a range of reserved memory
// Parameters:  range - The memory range this must match the range provided by physGetRsvdMemRange(x)
// Returns:     The start address of the memory range, or MEM_ADDRESS_ERROR if failed
extern RsvdAddr_t pmmAllocRsvdRange(RsvdMemRange_t range) {
    // make sure it is a reserved memory range
    if (!physIsRsvdMemRange(range)) return (RsvdAddr_t) MEM_ADDRESS_ERROR;

    // make sure whole range is not in use
    for (uint32_t page = AddressToPageOff(range.startaddr); page <= AddressToPageOff(range.endaddr); page++) {
        if (pmmMemInUse(page)) return (RsvdAddr_t) MEM_ADDRESS_ERROR;
    }

    // allocate the whole range
    for (uint32_t page = AddressToPageOff(range.startaddr); page <= AddressToPageOff(range.endaddr); page++) {
        if (!pmmSetInUse(page)) {
            // undo it
            for (uint32_t undopage = AddressToPageOff(range.startaddr); undopage < page; undopage++) {
                // purposely ignoring return code
                pmmClearInUse(undopage);
            }
            return (RsvdAddr_t) MEM_ADDRESS_ERROR;
        }
    }

    // if we get here we were successful
    return range.startaddr;
}

// Free a reserved memory range
// Parameters:  rsvdaddr -- The first address in the range to free, must match the value given by physAllocRsvdRange()
// Returns:     true if successful, false otherwise
extern bool pmmFreeRsvdRange(RsvdMemRange_t range) {
    // make sure it is a reserved memory range
    if (!physIsRsvdMemRange(range)) return false;

    // make sure the range is in use
    for (uint32_t page = AddressToPageOff(range.startaddr); page < AddressToPageOff(range.endaddr); page++) {
        if (!pmmClearInUse(page)) {
            //undo it
            for (uint32_t undopage = 0; undopage < page; undopage++) {
                // purposely ignoreing return code
                pmmClearInUse(undopage);
                return false;
            }
        }
    }

    return true;
}

// Initialize Physical Memory
// Parameters:  None
// Returns:     true if successful, false otherwise
bool initPMM(const multiboot_mem_map_info_t * const mmap) {
 
    // check parameters 
    if (!mmap) return false;            // null pointer
    if (!mmap->count) return false;     // no map

        bool rtncde = true;

    // initialize memory information
    PhysMemInfo.numPagesExist = 0;
    PhysMemInfo.numPagesInUse = 0;
    PhysMemInfo.numPagesAvail = 0;
    PhysMemInfo.numPagesRsvd = 0;
    PhysMemInfo.numPagesAvailInUse = 0;
    PhysMemInfo.numPagesRsvdInUse = 0;
    PhysMemInfo.numPagesDefect = 0;

    numRsvdMemRanges = 0;
    // set up the inital memory map
    rtncde &= physProcessMultibootMemMap(mmap);

    // set up video memory (not reported in the multiboot info)
    rtncde &= physProcessVideoMemory();

    // initialize the page tables
    rtncde &= initPageTables();

    // get inuse memory
    uint32_t errors = pmmSetInUsePhysicalMemory();
    if (errors) {
        printf("%ul errors found in setInUsePhysicalMemory()\n\r", errors);
        rtncde = false;
    }

    // get number of pages available and in use, and reserved and in use, etc
    for (uint32_t page = 0; page < 0x100000; page++) {
        if (pmmMemExists(page)) PhysMemInfo.numPagesExist++;
        if (pmmMemAvail(page)) PhysMemInfo.numPagesAvail++;
        if (pmmMemRsvd(page)) PhysMemInfo.numPagesRsvd++;
        if (pmmMemInUse(page)) {
            PhysMemInfo.numPagesInUse++;
            if (pmmMemAvail(page)) {
                PhysMemInfo.numPagesAvailInUse++;
            } else {
                PhysMemInfo.numPagesRsvdInUse++;
            }
        }
    }
    // set pointer to the next page offset to look at
    nextPhysPageOff = firstPhysPageOff;

    return rtncde;

}

// align a page address up, IF it is not on a page boundry
// Parameters:  addr -- the address to align
// Returns:     The aligned address
MemAddr_t PageAlignUp(MemAddr_t addr) {
    // is it already page aligned
    if ((addr & 0xFFFFF000) == addr) return addr;

    // not aligned allready, add a page and align it
    addr = (addr & 0xFFFFF000) + MEM_PAGE_SIZE;

    return addr;
}

// align a page address down, IF it is not on a page boundry
// Parameters:  addr -- the address to align
// Returns:     The aligned address
extern MemAddr_t PageAlignDown(MemAddr_t addr) {
    return addr & 0xFFFFF000;
}

// flush the Translation Lookaside Buffer (TLB)
// Parameters:  none
// returns;     None
void     FlushTLB() {
    // holds the CR3 register
    uint32_t cr3;

    __asm__ volatile (
        "mov %%cr3, %0\n\t"
        "mov %0, %%cr3"
        : "=r"(cr3)
        :
        : "memory"
    );
}

// invalidate an individual page
// Parameters:  virtaddress -- The virtual address of the page to be invalidated
// Returns:     None
void invalidatePage(VirtAddr_t * virtaddress) {
    __asm__ volatile ("invlpg (%0)" : : "r"(virtaddress) : "memory");
}