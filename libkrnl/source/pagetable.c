// page table functions
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include <pagetable.h>

extern uint32_t page_directory[1024];



// constants
pagetable_array_t * const    PAGETABLE = (pagetable_array_t *) PAGE_TABLE_START;


// get the page directory entry from a virtual address
pde_t getPDEFromAddress(uint32_t virtaddr) {
    return virtaddr >> 22;
}

// get the page table enry from a virtual address
pde_t getPTEFromAddress(uint32_t virtaddr) {
    return (virtaddr >> 12) & 0x3FF;
}

// initialize the page directory
// this must happen after we do multiboot, because it clears page 0

// currently it just clears PDE 0, except for the first entry 
// where the SYSCALL table will live

bool initPageDirectory() {

    // clear legacy Upper Memory Area from mapping (0xA0000-0xF0000)
    for (uint32_t i = 160; i < 240; i++) {
        (*PAGETABLE)[0][i] = 0;
    }

    // clear for page table 1022
    uint32_t * start = (uint32_t *) (PAGE_TABLE_START + (1022*0x1000));
    for (uint32_t i = 992; i < 1000; i++) {
        start[i] = 0;
    }

    FlushTLB();

    return true;
}

// get the virtual address of a particular page table
// Parameters:  pde -- The page directory entry of the table
// Returns:     The virtual address of the page table
uint32_t *getDirectoryTableVirtAddress(pde_t pde) {
    return (uint32_t *) &PAGETABLE[pde][0]; // (uint32_t *)(PAGE_TABLE_START + (pde << 12));
}

// get the page table physical address from a PDE,PDT
// Parameters:  pde - The page direcory entry
//              pte - The table table entry
// returns:     The physical address of the combination, or 0xFFFFFFF if failed

extern uint32_t getPageTablePhysAddress(uint32_t pde, uint32_t pte) {
    // check paramters
    if (pde > 1023) return 0xFFFFFFFF;
    if (pte > 1023) return 0xFFFFFFFF;

    // make sure we have a page directory entry
    if (!page_directory[pde]) return 0xFFFFFFFF;
    
    // do we have a page table entry
    if (!(*PAGETABLE)[pde][pte]) return 0xFFFFFFFF;

    return ((*PAGETABLE)[pde][pte] & 0xFFFFF000);
}

// get the virtual address of a particular page table
// Parameters:  pde -- The page directory entry of the table
// Returns:     The virtual address of the page table
extern uint32_t *getPageTableVirtAddress(pde_t pde, pte_t pte) {
        // check paramters
    if (pde > 1023) return (uint32_t *) 0xFFFFFFFF;
    if (pte > 1023) return (uint32_t *) 0xFFFFFFFF;


    //uint32_t *virtaddr = (uint32_t *)((uint32_t) PAGETABLE + (0x1000 * pde) + (pte * sizeof(uint32_t)));

    return &(*PAGETABLE)[pde][pte];
}

// set page table entry to a physical page
// Parameters:  virtaddr - the virtual address of the entry
//              physaddr -- the physical memory address to put in entry

// return:      true if successful, false if the page table entry already exists
bool setPageTableEntry(uint32_t virtaddr, uint32_t physaddr) {
    pde_t pde = getPDEFromAddress(virtaddr);
    pte_t pte = getPTEFromAddress(virtaddr);

    // validate we have a page directory entry first
    if (!page_directory[pde]) return false;

    // make sure the entry is alreay zero
    if ((*PAGETABLE)[pde][pte] != 0) return false;

    // set physical address to present and read/write
    physaddr &= 0xFFFFF000;
    physaddr += 3;
    
    // set the address
    (*PAGETABLE)[pde][pte] = physaddr;

    return true;

}

/// clear page table entry to a physical page
// Parameters:  virtaddr - the virtual address of the entry

// return:      true if successful, false if the page table entry doesn't exists
bool clearPageTableEntry(uint32_t virtaddr) {
    pde_t pde = getPDEFromAddress(virtaddr);
    pte_t pte = getPTEFromAddress(virtaddr);

    // validate we have a page directory entry first
    if (!page_directory[pde]) return false;

    // if it isn't in use fail
    if (!(*PAGETABLE)[pde][pte]) return false;
    // set the address
    (*PAGETABLE)[pde][pte] = 0;

    return true;
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
void     invalidatePage(void * virtaddress) {
    asm volatile("invlpg (%0)" : : "r"(virtaddress) : "memory");


}
