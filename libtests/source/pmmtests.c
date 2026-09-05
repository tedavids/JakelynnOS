// tests for physical memory manager
#include <stdio.h>
#include <multiboot.h>

#include <pmm.h>

// the two multiboot structures we will be using

static struct multiboot_memory_info_t meminfo;
static struct multiboot_mem_map_info_t mmap;


bool pmmtests() {

    // check base functions first

    
    bool rtncde = true;
    // start basic tests

    uint32_t tmpaddr = 0;
   
    // physical address of page directory entries
    if (physAddrOfPDE(&tmpaddr, 1024)) {
        print("physAddrOfPDE() accepted an out of range pde\n\r");
        rtncde = false;
    }

    if (physAddrOfPDE(nullptr, 1023)) {
        print("physAddrOfPDE() accepted a nullptr for address\n\r");
        rtncde = false;
    }

    if (!physAddrOfPDE(&tmpaddr, 1023)) {
        print("physAddrOfPDE(&tmpaddr, 1023) didn't find the address of entry 1023\n\r");
        rtncde = false;
    } else {
        if (tmpaddr != 0x119000) {
            printf("physAddrOfPDE(&tmpaddr, 1023) didn't return 0x11900, returned 0x%Xl\n\r",tmpaddr);
            rtncde = false;
        }
    }

 
    uint32_t pde = 1023;
    uint32_t pte = 3;

    // addrFromPdePdt
    if (physAddrFromPdePdt(nullptr,0,0)) {
        print("addrFromPdePdt(nullptr, 0,0) accepted a null pointer\n\r");
        rtncde = false;
    }
    if (physAddrFromPdePdt(&tmpaddr,1024,0)) {
        printf("physAddrFromPdePdt(&tmpaddr,1024,0) accepted an out of range pde\n\r");
        rtncde = false;
    }
    if (physAddrFromPdePdt(&tmpaddr, 0, 1024)) {
        print("physAddrFromPdePdt(&tmpaddr, 0, 1024) accepte an out of range pte\n\r");
        rtncde = false;
    }
    if (!physAddrFromPdePdt(&tmpaddr, pde, pte)) {
        printf("addrFromPdePte(&tmpaddr, %ul,%ul) failed.\n\r", pde, pte);
        rtncde = false;
    } else {
        if (tmpaddr != page_directory[3]) {
            printf("addrFromPdePte(&tmpaddr, %ul,%ul) failed. should be 0x%Xl, returned 0x%Xl\n\r", pde, pte, page_directory[3], tmpaddr);
            rtncde = false;        
        }
    }
    pte = 0;
    pde = 0;
    if (!physAddrFromPdePdt(&tmpaddr, pde, pte)) {
        printf("addrFromPdePte(&tmpaddr, %ul,%ul) failed \n\r", pde, pte);
        rtncde = false;
    } else {
        if (tmpaddr != 0x0) {
            printf("addrFromPdePte(&tmpaddr, %ul,%ul) failed. should be 0x0, returned 0x%Xl\n\r", pde, pte, tmpaddr);
            rtncde = false;
        }
    }

    // AddressToPge
    if (AddressToPage(0) != 0) {
        printf("AddressToPage(0) failed, should be 0, returned: %xl\n\r", AddressToPage(0));
        rtncde = false;
    }
    if (AddressToPage(0xFFFFFFFF) != 0xFFFFF) {
        printf("AddressToPage(0xFFFFFFFF) failed, should be 0xFFFFF, returned: %xl\n\r", AddressToPage(0xFFFFFFFF));
        rtncde = false;
    }
    if (AddressToPage(0xC1234567) != 0xC1234) {
        printf("AddressToPage(0xC1234567) failed, should be 0xC1234, returned: %xl\n\r", AddressToPage(0xC1234567));
        rtncde = false;
    }

    if (getPDEFromAddress(0xC0008000) != 0x300) {
        printf("getPDEFromAddress(0xC0008000) failed. Returned 0x%xl, expected 0x300\n\r",getPDEFromAddress(0xC0008000));
        rtncde = false;
    }

    if (getPTEFromAddress(0xF01B478A) != 436) {
        printf("getPTEFromAddress(0xF01B478A) failed.  Returned 0x%xl, expected 0x1B4\n\r", getPTEFromAddress(0xF01B478A));
        rtncde = false;
    }

    if (getPageTablePhysAddress(0,0) != (PAGETABLE[0] & 0xFFFFF000)) {
        printf("getPageTablePhysicalAddress(0,0) failed returned 0x%Xl, expected 0x%Xl\n\r", 
            getPageTablePhysAddress(0,0), PAGETABLE[0] & 0xFFFFF000);
        rtncde = false;
    }

    if (getPageTableVirtAddress(768,0) != 0xC0000000) {
        printf("getPageTableVirtAddress(768,0) failed, returned 0x%Xl, expected 0xC0000000\n\r", getPageTableVirtAddress(768,0));
        rtncde = false;
    }
    // if base functions don't work, pointless to test init
    if (!rtncde) return false;
 
    // set of our structures
    // we will have 3 memory areas

    // type 1 is available
    // type 2 is reserved

    // Type Start       End         Pages
    //  1   0x0         0x2FFF      3
    //  2   0x7000      0x9FFF      3
    //  1   0x10000     0x12FFF     3

    meminfo.lower = 0x6000; // 6 pages 3 available 3 reserved
    meminfo.upper = 0x3000; // 3 pages

    mmap.count = 3; // we have 3 regions
    // region 1
    mmap.region[0].baseaddr = 0x0;
    mmap.region[0].endaddr = 0x2FFF;
    mmap.region[0].memtype = 1;

    // region 2
    mmap.region[1].baseaddr = 0x7000;
    mmap.region[1].endaddr = 0x9FFF;
    mmap.region[1].memtype = 2;

    // region 3
    mmap.region[2].baseaddr = 0x10000;
    mmap.region[2].endaddr = 0x12FFF;
    mmap.region[2].memtype = 1;

    rtncde = processMultibootMemMap(&mmap);
    if (!rtncde) {
        print("processMultibootMemMap() failed\n\r");
    }


    // check simple functions

    // this sould have worked
    // check setInUsePhysicalMemory()
    auto errors = setInUsePhysicalMemory();
    // we are expecting ### errors
    if (errors != 1094) {
        printf("setInUsePhysicalMemory() failed, should have had 1094 error, returned %ul\n\r", errors);
    }

    
    // check that PhysMemory is set correctly
    // locations 0x0-0x2FFF should show available
    for (uint32_t addr = 0x0; addr < 0x2FFF; addr+= 0x1000) {
        if (!doesPhysMemExist(AddressToPage(addr))) {
            printf("!doesPhysMemoryExist(%ul) failed\n\r", addr);
            rtncde = false;
        }
        if (!isPhysMemAvail(AddressToPage(addr))) {
            printf("!isPhysMemAvail(%ul) afiled\n\r", addr);
            rtncde = false;
        }
        if (isPhysMemReserved(AddressToPage(addr))) {
            printf("isPhysMemReserved(%ul) failed\n\r", addr);
            rtncde = false;
        }
    }

   // locations 0x3000-0x9FFF should show not existing
    for (uint32_t addr = 0x3000; addr < 0x6FFF; addr+= 0x1000) {
        if (doesPhysMemExist(AddressToPage(addr))) {
            printf("doesPhysMemoryExist(%ul) failed\n\r", addr);
            rtncde = false;
        }
        if (isPhysMemAvail(AddressToPage(addr))) {
            printf("isPhysMemAvail(%ul) afiled\n\r", addr);
            rtncde = false;
        }
        if (isPhysMemReserved(AddressToPage(addr))) {
            printf("isPhysMemReserved(%ul) failed\n\r", addr);
            rtncde = false;
        }
    }

   // locations 0x7000-0x9FFF should show reserved
    for (uint32_t addr = 0x7000; addr < 0x9FFF; addr+= 0x1000) {
        if (!doesPhysMemExist(AddressToPage(addr))) {
            printf("!doesPhysMemoryExist(%ul) failed\n\r", addr);
            rtncde = false;
        }
        if (isPhysMemAvail(AddressToPage(addr))) {
            printf("isPhysMemAvail(%ul) afiled\n\r", addr);
            rtncde = false;
        }
        if (!isPhysMemReserved(AddressToPage(addr))) {
            printf("!isPhysMemReserved(%ul) failed\n\r", addr);
            rtncde = false;
        }
     }

       // locations 0x10000-0x12FFF should show available
    for (uint32_t addr = 0x10000; addr < 0x12FFF; addr+= 0x1000) {
        if (!doesPhysMemExist(AddressToPage(addr))) {
            printf("!doesPhysMemoryExist(%ul) failed\n\r", addr);
            rtncde = false;
        }
        if (!isPhysMemAvail(AddressToPage(addr))) {
            printf("!isPhysMemAvail(%ul) afiled\n\r", addr);
            rtncde = false;
        }
        if (isPhysMemReserved(AddressToPage(addr))) {
            printf("isPhysMemReserved(%ul) failed\n\r", addr);
            rtncde = false;
        }
     }

     // check physical memory in use
    if (!isPhysMemInUse(AddressToPage(0x2))) {
        print("!isPhysMemInUse(0x2) failed\n\r");
        rtncde = false;
    }
    if (!isPhysMemInUse(0x7)) {
        print("!isPhysMemInUse(0x7) failed\n\r");
        rtncde = false;
    }

    if (!isPhysMemInUse(0x10)) {
        print("!isPhysMemInUse(0x10) failed\n\r");
        rtncde = false;
    }    

    uint32_t addr = 0;
    if (!allocPhysMem(&addr)) {
        printf("allocPhysMem(&addr) failed\n\r");
        rtncde = false;
    } else {
        if (!addr) {
            printf("allocPhysMem(&addr) failed, returned 0x0\n\r");
            rtncde = false;
        }
    }

    if (!multiAllocPhysMem(addr)) {
        print("multiAllcoPhysMem(addr) failed\n\r");
        rtncde = false;
    }
    
    return rtncde;
}