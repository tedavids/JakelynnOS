// virtual Memory Manager

#include <string.h>
#include <stdio.h>
#include <vga.h>    // for start of vga memory (top of heap)
#include <pmm.h>

#include <vmm.h>

#define VIRT_MEMORY_NOT_SWAPPABLE   0b00000001
#define VIRT_MEMORY_SWAPPED_OUT     0b00000010
#define VIRT_MEMORY_READ_ONLY       0b00000100
#define VIRT_MEMORY_KERNEL          0b00001000
#define VIRT_MEMORY_SHARED          0b00010000
#define VIRT_MEMORY_IN_USE          0b00100000

#define PAGE_TABLES_ADDRESS         0x00100000

#define FLAG_PRESENT                0b00000001
#define FLAG_READWRITE              0b00000010
#define FLAG_PRIV_USER              0b00000100
#define FLAG_WRITE_THROUGH          0b00001000
#define FLAG_CACHE_DISABLED         0b00010000
#define FLAG_ACCESSED               0b00100000
#define FLAG_4M_PAGE                0b01000000
#define FLAG_GLOBAL                 0b10000000

// pre defined page tables
extern uint32_t PAGETABLE000[1024];
extern uint32_t PAGETABLEFF8[1024];
extern uint32_t PAGETABLEC00[1024];

// sections so we can set read/write flags correctly
extern const uint32_t _boot_kernel_start;
extern const uint32_t _text_start;
extern const uint32_t _text_end;
extern uint32_t _rodata_start;
extern uint32_t _rodata_end;
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern const uint32_t _heap_start;

// Variables
struct VirtMemInfo_t VirtMemInfo;
uint8_t  VirtMemoryPageStatus[0x100000];

uint32_t lowAvailKernelPage;
uint32_t highAvailKernelPage;
uint32_t nextAvailKernelPage;

// get virtual address from a page directory enty and page table entry

// get virtual address from a page directory enty and page table entry

// Parameters:  pde - The page direcory entry index
//              pte - The page table entry index

// Returns:     The virtual address of the entries
//              or 0xFFFFFFFF if it fails

uint32_t virtAddrFromPdePdt(uint32_t pde, uint32_t pte) {
    // check parameters
    if (pde > 1023) return 0xFFFFFFFF;
    if (pte > 1023) return 0xFFFFFFFF;

    // get the address
    return (pde << 22) + (pte << 12);
}

// get if virtual page is swapped out
bool isVirtMemSwappedOut(uint32_t virtaddr) {
    // if its not in use, it can't be swapped out
    if (!isVirtMemInUse(virtaddr)) return false;

    // check if it is swapped out
    return (VirtMemoryPageStatus[AddressToPage(virtaddr)] & VIRT_MEMORY_SWAPPED_OUT);
}

// set virtual memory to swapped out
bool setVirtMemSwappedOut(uint32_t virtaddr) {
    // if its not in use, it can't be swapped out
    if (!isVirtMemInUse(virtaddr)) return false;
    // if it isn't swappable, it can't be swapped out
    if (isVirtMemSwappable(virtaddr)) return false;
    // if it is already swapped out, we can't swap it out again
    if (isVirtMemSwappedOut(virtaddr)) return false;

    // set it swapped out
    VirtMemoryPageStatus[AddressToPage(virtaddr)] |= VIRT_MEMORY_SWAPPED_OUT;

    return true;

}

// clear virtual memory swapped out flag
bool clearVirtMemSwappedOut(uint32_t virtaddr) {
    // if its not in use, it can't be swapped out
    if (!isVirtMemInUse(virtaddr)) return false;
    // if it isn't swappable, it can't be swapped out
    if (isVirtMemSwappable(virtaddr)) return false;
    // if it is not swapped out, we can't swap clear it
    if (!isVirtMemSwappedOut(virtaddr)) return false; 

    // clear the swap flag
    VirtMemoryPageStatus[AddressToPage(virtaddr)] ^= VIRT_MEMORY_SWAPPED_OUT;

    return true;
}

// get if virtual memory is swappable
bool isVirtMemSwappable(uint32_t virtaddr) {
    // if its not in use it can't be swappable
    if (!isVirtMemInUse(virtaddr)) return false;

    bool temp = !(VirtMemoryPageStatus[AddressToPage(virtaddr)] & VIRT_MEMORY_NOT_SWAPPABLE);

    return temp;
}

// set virtual memory to swappable
bool setVirtMemSwappable(uint32_t virtaddr) {
    // if its not in use, it can't be swappable
    if (!isVirtMemInUse(virtaddr)) return false;
    // if it is already swappable it can't be set that way
    if (!isVirtMemSwappable(virtaddr)) return false;
    // set it swappable
    VirtMemoryPageStatus[AddressToPage(virtaddr)] &= (uint8_t)~(VIRT_MEMORY_NOT_SWAPPABLE);

    return true;
}

// clear virtual memory swapable
bool clearVirtMemSwappable(uint32_t virtaddr) {
    // if its not in use, we can't clear the flag
    if (!isVirtMemInUse(virtaddr)) return false;
    // if it is already not swappable it can't be set that way
    if (!isVirtMemSwappable(virtaddr)) return false;
    // if it is swapped out, we can't clear it either
    if (isVirtMemSwappedOut(virtaddr)) return false;
    // clear swapable
    VirtMemoryPageStatus[AddressToPage(virtaddr)] |= VIRT_MEMORY_NOT_SWAPPABLE;

    return true;
}

// get if virtual memory is in use
bool isVirtMemInUse(uint32_t virtaddr) {
    return (VirtMemoryPageStatus[AddressToPage(virtaddr)] & VIRT_MEMORY_IN_USE);
}

// set virtual memory to in use
bool setVirtMemInUse(uint32_t virtaddr) {
    // if it is already in use we have an issue
    if (isVirtMemInUse(virtaddr)) return false;

    // set it to in use
    VirtMemoryPageStatus[AddressToPage(virtaddr)] |= VIRT_MEMORY_IN_USE;
    return true;
}

// clear virtual memory in use flag
bool clearVirtMemInUse(uint32_t virtaddr) {
    // if memory is not in use we have an issue
    if (!isVirtMemInUse(AddressToPage(virtaddr))) return false;

    VirtMemoryPageStatus[AddressToPage(virtaddr)] ^= VIRT_MEMORY_IN_USE;
    return true;
}

// get if virtual page is read only
bool isVirtMemReadOnly(uint32_t virtaddr) {
    return (VirtMemoryPageStatus[AddressToPage(virtaddr)] & VIRT_MEMORY_READ_ONLY);
}

// set virtual memory to read only
// Parameters:  virtaddr -- address you wish to set read only
// Returns:     true if successful, false otherwise
bool setVirtMemReadOnly(uint32_t virtaddr) {
    // if virtual memory is already read only, we can't do it again
    if (isVirtMemReadOnly(virtaddr)) return true;

    // set it
    VirtMemoryPageStatus[AddressToPage(virtaddr)] |= VIRT_MEMORY_READ_ONLY;

    return true;

}

// clear virtual memory read only flag
bool clearVirtMemReadOnly(uint32_t virtaddr) {
    // if virtual memory is not read only, we can't clear it
    if (!isVirtMemReadOnly(virtaddr)) return true;

    // clear the flag
    VirtMemoryPageStatus[AddressToPage(virtaddr)] ^= VIRT_MEMORY_READ_ONLY;
    return true;
}

// get if virtual page is part of the kernel
bool isVirtKrnlMem(uint32_t virtaddr) {
    return (VirtMemoryPageStatus[AddressToPage(virtaddr)] & VIRT_MEMORY_KERNEL);
}

// set it as kernel memory
bool setVirtKrnlMem(uint32_t virtaddr) {
    // check it isn't already kernel
    if (isVirtKrnlMem(virtaddr)) return false;

    // set it as kernel memory
    VirtMemoryPageStatus[AddressToPage(virtaddr)] |= VIRT_MEMORY_KERNEL;
    return true;
}

// clear kernel memory flag
bool clearVirtKrnlMem(uint32_t virtaddr) {
    // check it's kernel
    if (!isVirtKrnlMem(virtaddr)) return false;

    VirtMemoryPageStatus[AddressToPage(virtaddr)] ^= VIRT_MEMORY_KERNEL;
    return true;
}


// get if virtual memory is shared
// Parameters:  virtaddr -- the address to check
// Returns:     true if shared, false if not or there is an issue
bool isVirtMemShared(uint32_t virtaddr) {
    // if it isn't in use, it can't be shared
    if (!isVirtMemInUse(virtaddr)) return false;

    return (VirtMemoryPageStatus[AddressToPage(virtaddr)] & VIRT_MEMORY_SHARED);
}

// set virtual memory to shared
bool setVirtMemShared(uint32_t virtaddr) {
    // if the memory isn't in use, it can't be shared
    if (!isVirtMemInUse(virtaddr)) return false;
    // is it already shared?
    if (isVirtMemShared(virtaddr)) return false;

    VirtMemoryPageStatus[AddressToPage(virtaddr)] |= VIRT_MEMORY_SHARED;

    return true;
}

// clear shared bit
bool clearVirtMemShared(uint32_t virtaddr) {
   // if the memory isn't in use, it can't be shared
    if (!isVirtMemInUse(virtaddr)) return false;
    // is it shared?
    if (!isVirtMemShared(virtaddr)) return false;

    VirtMemoryPageStatus[AddressToPage(virtaddr)]  ^= VIRT_MEMORY_SHARED;

    return true;
}

// get the next available kernel address
// Parameters:  None
// returns:     the next available kernel page, or zero if out of memory
uint32_t getNextAvailKernelPage() {
    for (uint32_t page = nextAvailKernelPage; page > lowAvailKernelPage; page--) {
        // see if page is in use
        if (!isVirtMemInUse(page << 12)) {
            // move to next page
            nextAvailKernelPage--;
            if (nextAvailKernelPage == lowAvailKernelPage) {
                nextAvailKernelPage = highAvailKernelPage;
            }
            return page;
        }
    }
    // we ran into the beginning of the buffer, try from the back
    for (uint32_t page = lowAvailKernelPage; page > highAvailKernelPage; page--) {
        // see if page is in use
        if (!isVirtMemInUse(page << 12)) {
            // move to next page
            nextAvailKernelPage--;
            if (nextAvailKernelPage == lowAvailKernelPage) {
                nextAvailKernelPage = highAvailKernelPage;
            }
            return page;
        }        
    }
    // if we get here we are out of memory
    return 0;
}

// allocate kernel virtual memory
// Parameters:  virtaddr -- the address you wish allocated
//              invalidatepage -- do you want to invalidate the page
// Returns:     returns the allocated address if successful, nullptr otherwise
uint32_t* allocVirtKrnlMem(uint32_t virtaddr, bool invalidatepage) {
    // check memory isn't already in use
    if (isVirtMemInUse(virtaddr)) return nullptr;

    // allocate the memory
    if (!allocVirtMem(virtaddr,invalidatepage)) return nullptr;

    if (!setVirtKrnlMem(virtaddr)) printf("Error setting virtuall kernel memory: 0x%Xl\n\r",virtaddr);

    return (uint32_t *) virtaddr;

}

// deallocate kernel virtual memory
// Parameters:  virtaddr -- the address you wish allocated
//              invalidatepage -- do you want to invalidate the page
// Returns:     true if successfull, false if unsuccessful
bool deallocVirtKrnlMem(uint32_t virtaddr, bool invalidatepage) {
    // check parameters
    if (!isVirtMemInUse(virtaddr)) return false;
    if (!isVirtKrnlMem(virtaddr)) return false;

    // its ok to deallocate
    if (!deallocVirtMem(virtaddr,invalidatepage)) return false;

    // clear the kernel flag
    if (!clearVirtKrnlMem(virtaddr)) printf("Error clearing virtual kernel memory flag: 0x%Xl\n\r",virtaddr);

    return true;
}

// get page table address
// parameters:  pde
// returns:     the address of the page table, or 0xFFFFFFFF if error
uint32_t *getPageTable(uint32_t pde) {
    switch (pde) {
        case 0: 
            return PAGETABLE000;
         case 768:
            return PAGETABLEC00;
        default:
            return &PAGETABLE[pde];
    }
}

// alloc virtual memory anonomously
// Parameters:  invalidatepage -- invalidate the page
// Returns:     a pointer to the address if successful, nullptr otherwise

uint32_t* kallocAnonKrnlMem(bool invalidatepage) {
    // get next available kernel page
    uint32_t virtaddr = getNextAvailKernelPage();
    // are we out of memory?
    if (!virtaddr) return nullptr;
  
    
    // allocate the page
    if (!allocVirtMem(virtaddr, invalidatepage)) {
        printf("Problem allocating anonomous memory\n\r");
    }

    // set it to kernel memory
    setVirtKrnlMem(virtaddr);

    return (uint32_t *) virtaddr;
    
}

// allocate virtual memory
// Parameters:  virtaddr -- the address you wish allocated
//              invalidatepage -- do you want to invalidate the page
// Returns:     a pointer to the memory allocted or nullptr if it fails
uint32_t* allocVirtMem(uint32_t virtaddr, bool invalidatepage) {
    // if it is already allocated you should use the shared alloc function
    if (isVirtMemInUse(virtaddr)) return nullptr;

    // first see if we have a PDE for the address
    pde_t pde = getPDEFromAddress(virtaddr);
    
    // this is fall back code it should never be called
    if (!page_directory[pde]) {
        printf("Missing page directory entry: %ul\n\r",pde);
        // we need to make a page table
        // we know it wont be page 0, 766, or 767
        bool complete = allocPhysMem(&page_directory[pde]);
        // if we didn't get a page table we can't do this functino
        if (!complete) return nullptr;
    }

    // get pointer to the page table 
    uint32_t *pt = getPageTable(pde);
    pte_t pte = getPTEFromAddress(virtaddr);

    uint32_t physaddr;
    bool complete = allocPhysMem(&physaddr);
    // out of memory?
    if (!complete) return nullptr;
    // set address and flags for memory
    pt[pte] = (physaddr & 0xFFFFF000) & FLAG_PRESENT;
    // add read write if necessary
    if (!isVirtMemReadOnly(AddressToPage(virtaddr))) pt[pte] &= FLAG_READWRITE;
    // if it is part of the kernel, it shouldn't be flushed on task or context switch
    // otherwise it is user priority
    if (isVirtKrnlMem(AddressToPage(virtaddr))) {
        pt[pte] &= FLAG_GLOBAL;
    } else { 
        pt[pte] &= FLAG_PRIV_USER;}

    // set page to in use
    complete = setVirtMemInUse(virtaddr);

    // invalidate page if necessary
    if (invalidatepage) invalidatePage(&pt[pte]);

    return (uint32_t *) virtaddr;
}

// allocate a block of virtual memory
// Parameters:  startaddr -- the starting address you want to allocate
//              endaddr -- the ending address you want to allocate
//              *lastallocated -- the last page allocated on error, if 0 none were done

// returns:     a pointer to the memory allocated, or nullptr if it fails
uint32_t* allocVirtMemBlock(uint32_t startaddr, uint32_t endaddr, uint32_t *lastallocated) {
    // check parameters
    if (!lastallocated) return false;
    // we have it so set it to zero
    *lastallocated = 0;

    // is end address > start address
    if (endaddr <= startaddr) return nullptr;

    // check that our address range has no allocated memory
    for (uint32_t addr = startaddr; addr <= endaddr; addr += 0x1000) {
        if (isVirtMemInUse(addr)) return false;
    }

    // all are addresses are not allocated

    for (uint32_t addr = startaddr; addr <= endaddr; addr += 0x1000) {
        if (!allocVirtMem(addr, false)) {
            // we had an issue in allocation
            FlushTLB();
            return nullptr;
        }
        *lastallocated = addr;
    }

    // flush the transaction lookaside buffer
    FlushTLB();

    return (uint32_t*) startaddr;
}

// deallocate virtual memory
// Parameters:  virtaddr -- the address you with to deallocate
//                          it must have been allocated by allocVirtMem
//              invalidatepage -- do you want to invalidate the page
// Returns:     true if successful, false otherwise
bool deallocVirtMem(uint32_t virtaddr, bool invaliatepage) {
    // check that it is allocated
    if (!isVirtMemInUse(virtaddr)) return false;

    // first see if we have a PDE for the address
    pde_t pde = getPDEFromAddress(virtaddr);

    // no page directory entry, we can't deallocate
    if (!page_directory[pde]) return false;

    // get pointer to the page table 
    uint32_t *pt = getPageTable(pde);
    pte_t pte = getPTEFromAddress(virtaddr);

    // if the entry is 0 we have an issue
    if (!pt[pte]) return false;

    // deallocate the memory
    bool deallocated = deAllocPhysMem(pt[pte] & 0xFFFFF000);

    if (!deallocated) return false;

    pt[pte] = 0;

    if (!clearVirtMemInUse(virtaddr)) return false;

    if (invaliatepage) invalidatePage(&virtaddr);

    return deallocated;
}

// deallocates a block of virtual memory
// Parameters:  startaddr - The start address of the deallocation
//              endaddr -- the end address of the deallocation
//              lastdealloc -- the last address actually deallocated

// returns:     true if successful, false otherwise
bool deallocVirtMemBlock(uint32_t startaddr, uint32_t endaddr, uint32_t *lastdealloc) {
    // check parameters
    if (!lastdealloc) return false;
    *lastdealloc = 0;
    // check that start is before end
    if (startaddr > endaddr) return false;

    // check that all memory is allocated
    for (uint32_t addr = startaddr; addr <= endaddr; addr += 0x1000) {
        if (!isVirtMemInUse(addr)) return false;
    }

    // we know all the memory is allocated
    for (uint32_t addr = startaddr; addr <= endaddr; addr += 0x1000) {
        if (!deallocVirtMem(addr, false)) {
            // failure on deallocation
            FlushTLB();
            return false;
        }
        *lastdealloc = addr;
    }

    FlushTLB();

    return true;
}

// allocate virtual memory
// Parameters:  virtaddr -- the address you wish multi allocated, it must already have been allocated by allocVirtMem
// Returns:     a pointer to the address or nullptr if it fails
uint32_t* allocSharedVirtMem(uint32_t addrtoshare, uint32_t virtaddr) {
    // make sure address to share is in use
    if (!isVirtMemInUse(addrtoshare)) return nullptr;
    // make sure address we want is not in use
    if (isVirtMemInUse(virtaddr)) return nullptr;

    // we are ok

    // first see if we have a PDE for the address
    pde_t pde = getPDEFromAddress(addrtoshare);

    // no page directory entry, we can't deallocate
    if (!page_directory[pde]) return nullptr;

    // get pointer to the page table 
    uint32_t *pt = getPageTable(pde);
    pte_t pte = getPTEFromAddress(addrtoshare);

    uint32_t physpte = pt[pte];
    // check if the page is in use
    if (!physpte) return nullptr;

    // now assign it to the new adderss
    pde = getPDEFromAddress(virtaddr);

    // if no pde, we have an issue
    if (!page_directory[pde]) return nullptr;

    //get the page table entry
    pte = getPTEFromAddress(virtaddr);
    // get the page table 
    pt = getPageTable(pde);

    // if it isn't zero we have an issue
    if (pt[pte]) return false;

    pt[pte] = physpte;

    // mark new memory as shared
    if (!setVirtMemShared(virtaddr)) return nullptr;

    // mark shared, its ok if it is already shared so ignore the return
    setVirtMemShared(addrtoshare);

    invalidatePage(&virtaddr);

    return (uint32_t *) virtaddr;

}   

// deallocate virtual memory
// Parameters:  virtaddr -- the address you with to deallocate
//                          it must have been allocated by allocSharedVirtMem, unless it was the first allocation
// Returns:     true if successful, false otherwise

// Note: The user is responsible to keep track of the number of times it's been shared
bool deallocSharedVirtMem(uint32_t virtaddr, bool lastaddr) {
    // make sure address is shared
    if (!isVirtMemShared(virtaddr)) return false;

    // get pde for address
    pde_t pde = getPDEFromAddress(virtaddr);

    // do I have a page table
    if (!page_directory[pde]) return false;

    // get the page table entry
    pte_t pte = getPTEFromAddress(virtaddr);
    uint32_t *pt = getPageTable(pde);

    // if we don't have memory allocated it's an issue 
    if (!pt[pte]) return false;

    // clear the shared flag
    if (!clearVirtMemShared(virtaddr)) return false;

    // if this is the last one we need to deallocate it
    if (lastaddr) {
        if (!deallocVirtMem(virtaddr,true)) return false;
        return true;
    }

    // mark the virtual address not in use
    clearVirtMemInUse(virtaddr);
    // clear the page table entry 
    pt[pte] = 0;

    invalidatePage(&virtaddr);

    return true;
}

// set in use virtual memory

uint32_t setInUseVirtualMemory() {
    // no parameters to check

    uint32_t numerrors = 0;

    for (pde_t pde = 0; pde < 1024; pde++) {
        // if the page directory has an entry we can check the page tables
        if (page_directory[pde]) {
            // get start address of page table
            // 1024*sizeof(uint32_t) is the size of a page table
            uint32_t temp = (uint32_t) (PAGETABLE) + (pde * 0x1000);
            uint32_t * ptaddr = (uint32_t *) temp;
            for (pte_t pte = 0; pte < 1024; pte++) {
                if (ptaddr[pte]) {
                    // if we get here the page table exists
                    uint32_t virtaddr = virtAddrFromPdePdt(pde,pte);
                    // if valid it should be in use, PMM should check to see if the physmem is ok
                    if (virtaddr != 0xFFFFFFFF) {
                        VirtMemoryPageStatus[AddressToPage(virtaddr)] |= VIRT_MEMORY_IN_USE;
                    } else {
                        numerrors++;
                    }
                } 
            } 
        }
    }

    return numerrors;
}

//void invalidatePage(void * virtaddress) {
//    asm volatile("invlpg (%0)" : : "r" (virtaddress) : "memory");
//}

// initialize the virtual memory manager, this must be call AFTER the initPMM function
bool initVMM() {
    // set available kernel pages
    highAvailKernelPage = 0xffbe7;
    lowAvailKernelPage = (uint32_t) &_heap_start >> 12;
    nextAvailKernelPage = highAvailKernelPage;

    bool rtncde = true;

    // initialize table
    memset(VirtMemoryPageStatus,0,sizeof(VirtMemoryPageStatus));

    // set kernel memory
    for (uint32_t page = ((uint32_t)(&_boot_kernel_start + 0xC00000) >> 12); (page <= 0xFFFFF) && (page != 0); page++) {
        VirtMemoryPageStatus[page] |= VIRT_MEMORY_KERNEL;
    }

    // set Read only memory
    // kernel code is read only
    for (uint32_t page = ((uint32_t)&_text_start >> 12); page <= ((uint32_t)&_text_end >> 12); page++) {
        VirtMemoryPageStatus[page] |= VIRT_MEMORY_READ_ONLY;
        VirtMemoryPageStatus[page] |= VIRT_MEMORY_IN_USE;
    }

    // read only data is read only
    for (uint32_t page = (uint32_t)&_rodata_start >> 12; page <= ((uint32_t)&_rodata_end >> 12); page++) {
        VirtMemoryPageStatus[page] |= VIRT_MEMORY_READ_ONLY;
        VirtMemoryPageStatus[page] |= VIRT_MEMORY_IN_USE;
    }

    // set memory not swappable
    // video memory
    for (uint32_t page = 0xFF7E8; page <= 0xFF7FF; page++) {
        VirtMemoryPageStatus[page] |= (uint8_t)VIRT_MEMORY_NOT_SWAPPABLE;
    }

    // page tables
    for (uint32_t page = AddressToPage((uint32_t)PAGETABLE); (page <= 0xFFFFF) && (page != 0); page++) {
        VirtMemoryPageStatus[page] |= (uint8_t)VIRT_MEMORY_NOT_SWAPPABLE;
    }


    // walk the page table to get what's in use
    uint32_t errors = setInUseVirtualMemory();
    if (errors) {
        printf("setInUseVirtualMemory() had %ul error\n\r",errors);
        rtncde = false;
    }

    return rtncde;
}