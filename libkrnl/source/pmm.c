// Physical Memory Manager

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <multiboot.h>

#include <pmm.h>

// constants
#define PHYSICAL_MEMORY_EXISTS      0b00000001
#define PHYSICAL_MEMORY_AVAILABLE   0b00000010
#define PHYSICAL_MEMORY_IN_USE      0b00000100


#define RECURSIVE_PAGE_DIRECTORY_ADDR 0xFFFFF000

#define ADDRESS_MASK                0xFFFFF000
// externals
extern uint32_t PAGETABLE000[1024];
extern uint32_t PAGETABLEBF8[1024];
extern uint32_t PAGETABLEBFC[1024];
extern uint32_t PAGETABLEC00[1024];

// This shows the status of every page in memory
struct PhysMemInfo_t     PhysMemInfo;
uint8_t  PhysMemoryPageStatus[0x100000];

static uint32_t firstPhysOffset;
static uint32_t lastPhysOffset;
static uint32_t lastAllocOffset;

// our recursive page directory
uint32_t * const    recurPageDirectory = (uint32_t *) RECURSIVE_PAGE_DIRECTORY_ADDR;


// get page from address
uint32_t AddressToPage(uint32_t address) {
    return (address >> 12);
}

// get the physical address from the page directory and page table entry
// Returns:     The page table entry
bool physAddrFromPdePdt(uint32_t *address,uint32_t pde, uint32_t pte) {
    // validate parameters
    if ((!address) || (pde > 1023) || (pte > 1023)) {
        address = nullptr;
        return false;
    }

    volatile uint32_t *pt = (volatile uint32_t *)(PAGE_TABLE_START + (pde << 12));

    *address = pt[pte] & ADDRESS_MASK;

    return true;
}

// get PDE physical address
// Parameters:  address -- a pointer to where you want the physical address
//              pde -- the index in the page direcory
// Returns:     true if there is a phsical address, false otherwise
//              if true address is the physical address, otherwise address is a null pointer
bool physAddrOfPDE(uint32_t *address, pde_t pde) {
    // validate parameters
    if ((!address) || (pde > 1023)) {
        address = nullptr;
        return false;
    }
    // do we have a page directory entry
    if (!page_directory[pde]) {
        address = nullptr;
        return false;
    }

    // we have an address
    *address = page_directory[pde] & ADDRESS_MASK;

    return true;
}



// get if physical page exists
bool physMemExists(uint32_t phypageoffset) {
    return (PhysMemoryPageStatus[phypageoffset] & PHYSICAL_MEMORY_EXISTS);
}


// get if physical page is avaiable (not reserved)
bool physMemAvail(uint32_t phypageoffset) {
    // if page doesn't exists, it isn't available
    if (!physMemExists(phypageoffset)) return false;
    // if it does exist is it available
    return (PhysMemoryPageStatus[phypageoffset] & PHYSICAL_MEMORY_AVAILABLE);
}


// get if physical page is reserved
bool physMemReserved(uint32_t phypageoffset) {
    return (physMemExists(phypageoffset) & !physMemAvail(phypageoffset));
}

// get if physical page is allocated in a page table
bool physMemInUse(uint32_t phypageoffset) {
    // check if it exists
    if (!physMemExists(phypageoffset)) return false;
    // if it does exist is it in a page table
    return (PhysMemoryPageStatus[phypageoffset] & PHYSICAL_MEMORY_IN_USE);
}

// set physical memory allocated

// Parameters:  phypageoffset - the offset to the physical page 

// Returns:     true if page is set to allocated
//              fallse if the physical memory does not exist, or is already allocated
bool setPhysMemAlloc(uint32_t phypageoffset) {
    // if page doesn't exists, it isn't available
    if (!physMemExists(phypageoffset)) return false;
    // if it is already allocated it we don't have to set it allocated
    if (physMemInUse(phypageoffset)) return false;

    // set the memory as available
    PhysMemoryPageStatus[phypageoffset] |= PHYSICAL_MEMORY_IN_USE;
    // we have one more used
    PhysMemInfo.PagesInUse++;

    return true;
}

// set Physical Memory Not Allocated
// Parameters:  phypageoffset - the offset to the physical page 

// Returns:     true if page is set to allocated
//              fallse if the physical memory does not exist, or is already allocated
bool clearPhysMemInUse(uint32_t phypageoffset) {
    // if page doesn't exists, it isn't available
    if (!physMemExists(phypageoffset)) return false;
    // if it is already not allocated it we don't have to set it deallocated
    if (!physMemInUse(phypageoffset)) return false;

    // set the memory as available
    PhysMemoryPageStatus[phypageoffset] &= (uint8_t)~(PHYSICAL_MEMORY_IN_USE);
    // we have one less used
    PhysMemInfo.PagesInUse--;

    return true;
}


// get next free page offset

// parameters:  none

// returns the offset to the next free page, or 0xFFFFFFFF if out of memory

uint32_t getNextPhysFreePage() {
    // while we aren't at the end of the list and the memory already allocated
    while ((lastAllocOffset < lastPhysOffset)) {
        if (physMemAvail(lastAllocOffset) && !physMemInUse(lastAllocOffset)) {
            return lastAllocOffset++;
        }
        lastAllocOffset++;
    }
 
    // check the table again because we ran off the end
    // while we aren't at the end of the list and the memory already allocated
    lastAllocOffset = firstPhysOffset;
    while ((lastAllocOffset < lastPhysOffset)) {
        if (physMemAvail(lastAllocOffset) && !physMemInUse(lastAllocOffset)) {
            return lastAllocOffset++;
        }
        lastAllocOffset++;
    }
    // out of memory
    return 0xFFFFFFFF;
 
}

// allocate a page

// Parameters:  *physpage Address of page allocated

// Returns:     *physpage - address of phyical page allocated, or nullptr if not  allocated
//              the address of the physical page allocated, or 0xFFFFFFFF if it failed
bool allocPhysMem(uint32_t *physpage) {
    if (!physpage) return false;

    // get next free page
    *physpage = getNextPhysFreePage();

    // is it a valid page
    if (*physpage == 0xFFFFFFFF) {
        return nullptr;
    }

    bool rtncde = false;
    // if found, mark as allocated
    if (*physpage) {
        rtncde = setPhysMemAlloc(*physpage);
    }

    // make it an address
    *physpage <<= 12;

    return rtncde;
}

// Deallocate a page
bool deAllocPhysMem(uint32_t physpage) {
    // make it an offset
    physpage >>= 12;
    // does page exist
    if (!physMemExists(physpage)) return false;
    // is it allocated
    if (!physMemInUse(physpage)) return false;
    // it is in use
    clearPhysMemInUse(physpage);
    return true;
}

// allocate a physical page more than once
// note:  It is up to the caller to make sure it isn't deallocated prematurely
extern bool multiAllocPhysMem(uint32_t physpage) {
    if (!clearPhysMemInUse(physpage)) return false;

    return true;
}



// get the multiboot memory type
uint32_t getMemoryType(uint32_t type) {
    switch(type) {
        // available
        case 1: return 1;
        // reserved
        case 2: return 2;
        // ACPI reclaimable
        case 3: return 1;
        // defective
        case 4: return 4;
        // unknown
        default: return type;
    }
}


// process the multiboot memory map

// Parameters:  mmap -- the multiboot memory map
// Returns:     true if successful

bool processMultibootMemMap(const struct multiboot_mem_map_info_t * const mmap) {
    // initialize physical memory
    memset(PhysMemoryPageStatus,0,sizeof(PhysMemoryPageStatus));

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
    firstPhysOffset = (uint32_t) (mmap->region[firstavail].baseaddr >> 12);

    // last physical offset
    lastPhysOffset = (uint32_t) (mmap->region[mmap->count - 1].endaddr >> 12);

    // get last available physical memory 
    uint32_t lastavail = mmap->count - 1;
    while (lastavail > 0) {
        // check if region is available
        if (mmap->region[lastavail].memtype == 1) break;
        lastavail--;
    }

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
                startaddr = PAGE_ALIGN_UP((uint32_t)mmap->region[i].baseaddr);
                endaddr = (uint32_t)mmap->region[i].endaddr;
                break;
            // reserved
            case 2:
                startaddr = PAGE_ALIGN_DOWN((uint32_t)mmap->region[i].baseaddr);
                endaddr = (uint32_t)mmap->region[i].endaddr;
                break;
            // ACPI reclaimable (treat as type 1)
            case 3:
                startaddr = PAGE_ALIGN_UP((uint32_t)mmap->region[i].baseaddr);
                endaddr = (uint32_t)mmap->region[i].endaddr;
                break;
            // defective (skip)
            case 4:
                startaddr = 9999;
                endaddr = 0;
                break;
            // we don't know (skip it)
            default:
                startaddr = 9999;
                endaddr = 0;
                break;
        }

        for (uint32_t page = AddressToPage(startaddr);
            page <= AddressToPage(endaddr); page++) {
            // get page offset
            PhysMemoryPageStatus[page] |= PHYSICAL_MEMORY_EXISTS;   // memory exists
            PhysMemInfo.PagesExist++;

            // is it available to us or is it reserved, if it is reserved, it is also read only
            switch (getMemoryType(mmap->region[i].memtype)) {
                // available
                case 1:
                case 3:
                    PhysMemoryPageStatus[page] |= PHYSICAL_MEMORY_AVAILABLE;
                    PhysMemInfo.PagesAvail++;
                    lastAllocOffset = page;
                    break;
                // reserved
                case 2:
                    PhysMemInfo.PagesReserved++;
                    break;
                // ignore others
                default:
                    break;
            }
        }
    }
    lastAllocOffset = firstPhysOffset;
    return true;
}

// set in use memory flags

// Parameters:  None
// Returns:     the number of errors it found (should be 0)

uint32_t setInUsePhysicalMemory() {
    // no parameters to check

    uint32_t numerrors = 0;

    for (pde_t pde = 0; pde < 1024; pde++) {
        // if the page directory has an entry we can check the page tables
        if (page_directory[pde]) {
            // get start address of page table
            // 1024*sizeof(uint32_t) is the size of a page table
            uint32_t temp = PAGE_TABLE_START + (pde * 0x1000);
            uint32_t * ptaddr = (uint32_t *) temp;
            for (pte_t pte = 0; pte < 1024; pte++) {
                if (ptaddr[pte]) {
                    uint32_t phypage = AddressToPage(ptaddr[pte] & 0xFFFFF000);
                    // set inuse if it exists
                    if (physMemExists(phypage)) {
                        PhysMemoryPageStatus[phypage] |= PHYSICAL_MEMORY_IN_USE;
                    } else {
                        numerrors++;
                    }
                } 
            } 
        }
    }

    return numerrors;
}

// note this is hardcoded, we will change at some point

bool processVideoMemory() {
    // video memory shows as reserved (not available)
    // i is the offset (not the address)
    for (uint32_t i = 0xA8; i <= 0xBF; i++) {
        PhysMemoryPageStatus[i] |= PHYSICAL_MEMORY_EXISTS;
    }

    return true;
}
// This must be called AFTER the multiboot information is collected
// Initialize memory management

// Parameters: None

// Returns:     true if memory management structures are successfully initialized,
//              false otherwise

bool initPMM(const struct multiboot_memory_info_t * const meminfo,  const struct multiboot_mem_map_info_t * const mmap) {
    // check parameters
    if (!meminfo) return false;
    if (!mmap) return false;

    bool rtncde = true;

    // initialize memory information
    PhysMemInfo.PagesAvail = 0;
    PhysMemInfo.PagesInUse = 0;
    PhysMemInfo.PagesExist = 0;
    PhysMemInfo.PagesReserved = 0;



    // set up the inital memory map
    rtncde &= processMultibootMemMap(mmap);

    // set up video memory (not reported in the multiboot info)
    rtncde &= processVideoMemory();

    // get inuse memory
    uint32_t errors = setInUsePhysicalMemory();
    if (errors) {
        printf("%ul errors found in setInUsePhysicalMemory()\n\r", errors);
        //rtncde = false;
    }

    return rtncde;
}