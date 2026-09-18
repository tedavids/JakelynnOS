// tests for physical memory manager
#include <stdio.h>
#include <multiboot.h>

#include <pmm.h>

extern uint32_t stack_top;
extern uint32_t page_directory[1024];

bool pmmtests() {
   
    bool rtncde = true;
    // start basic tests
   // check base functions first
    if (PageOffToAddress(AddressToPageOff(0x12345000)) != 0x12345000) {
        printf("PageOffToAddress(AddressToPageOff(0x12345000)) failed, returned: 0x%Xl\n\r",
            PageOffToAddress(AddressToPageOff(0x12345000)));
        rtncde = false;
    }

    // test page alignment
    if (PageAlignDown(0x9c000) != 0x9c000) {
        printf("PageAlignDown(0x9c000) failed, returned 0x%xl\n\r",PageAlignDown(0x9c00));
        rtncde = false;
    }
    if (PageAlignDown(0x9c001) != 0x9c000) {
        printf("PageAlignDown(0x9c001) failed, returned 0x%xl\n\r",PageAlignDown(0x9c01));
        rtncde = false;
    }
    if (PageAlignDown(0x9cfff) != 0x9c000) {
        printf("PageAlignDown(0x9c00) failed, returned 0x%xl\n\r",PageAlignDown(0x9cff));
        rtncde = false;
    }
    if (PageAlignUp(0x9c000) != 0x9c000) {
        printf("PageAlignUp(0x9c000) failed, returned 0x%xl\n\r",PageAlignUp(0x9c000));
        rtncde = false;
    }
    if (PageAlignUp(0x9c001) != 0x9d000) {
        printf("PageAlignUp(0x9c001) failed, returned 0x%xl\n\r",PageAlignUp(0x9c001));
        rtncde = false;
    }
    if (PageAlignUp(0x9cfff) != 0x9d000) {
        printf("PageAlignUp(0x9cfff) failed, returned 0x%xl\n\r",PageAlignUp(0x9cfff));
        rtncde = false;
    }

    // reserved memory ranges
    auto numrsvdranges = pmmNumRsvdMemRanges();
    if (numrsvdranges != (uint32_t) 6) {
        printf("physNumRsvdMemRanges() did not return 6, returned %ul\n\r",pmmNumRsvdMemRanges());
        rtncde = false;
    }

    // Out of range
    auto range = pmmGetRsvdMemRange(numrsvdranges);
    if (range.startaddr != MEM_RANGE_ERROR) {
        printf("physGetRsvdMemRange(outofrange), passed validation returned: start 0x%Xl, end 0x%Xl\n\r",
            range.startaddr, range.endaddr);
        rtncde = false;
    }
    // range 1
    range = pmmGetRsvdMemRange(0);
    if ((range.startaddr != 0x9fc00) || (range.endaddr != 0x9ffff)) {
        printf("physGetRsvdMemRange(0) failed, should be (0x9fc00, 0x9ffff), returned (0x%xl,0x%xl)\n\r",
            range.startaddr, range.endaddr);
        rtncde = false;
    }
    // range 6
    range = pmmGetRsvdMemRange(numrsvdranges - 1);
    if ((range.startaddr != 0xFFFC0000) || (range.endaddr != 0xFFFFFFFF)) {
        printf("physGetRsvdMemRange(6) failed, should be (0xFFFC0000, 0xFFFFFFFF), returned (0x%xl,0x%xl)\n\r",
            range.startaddr, range.endaddr);
        rtncde = false;
    }

    // physical memory information
    auto meminfo = pmmGetPhysMemInfo();
    if (meminfo.numPagesExist != 0x2ff8a) {
        printf("meminfo.numPagesExist != 0x2ff8a, returned 0x%xl\n\r",meminfo.numPagesExist);
        rtncde = false;
    }
    if (meminfo.numPagesInUse != 0x13a) {
        printf("meminfo.numPagesinUse != 0x13a, returned 0x%xl\n\r",meminfo.numPagesInUse);
        rtncde = false;
    }
    if (meminfo.numPagesAvail != 0xfef9) {
        printf("meminfo.numPagesAvail != 0xfef9, returned 0x%xl\n\r",meminfo.numPagesAvail);
        rtncde = false;
    }
    if (meminfo.numPagesRsvd != 0x10053) {
        printf("meminfo.numPagesRsvd != 0x10053, returned 0x%xl\n\r",meminfo.numPagesRsvd);
        rtncde = false;
    }
    if (meminfo.numPagesDefect != 0x0) {
        printf("meminfo.numPagesDefect != 0x0, returned 0x%xl\n\r",meminfo.numPagesDefect);
        rtncde = false;
    }
    if (meminfo.numPagesAvailInUse != 0x121) {
        printf("meminfo.numPagesAvailInUse != 0x121, returned 0x%xl\n\r",meminfo.numPagesAvailInUse);
        rtncde = false;
    }
    if (meminfo.numPagesRsvdInUse != 0x19) {
        printf("meminfo.numPagesRsvdInUse != 0x10, returned %xl\n\r",meminfo.numPagesRsvdInUse);
        rtncde = false;
    }

    // test get phys mem size
    if (pmmMemSize() != 267358208) {
        printf("physMemSize() not 267358208, returned %ul\n\r", pmmMemSize());
        rtncde = false;
    }

    // test physAlloc()
    auto physaddr = pmmAlloc();
    if (!physaddr) {
        print("physAlloc() failed, returned 0\n\r");
        rtncde = false;
    } else {
        // not test free
        if (!pmmFree(physaddr)) {
            printf("physFree(0x%Xl) failed\n\r", physaddr);
            rtncde = false;
        }
    }

    // test rsvdAlloc()
    // try to allcate non reserved memory physaddr has an available address here
    physaddr = pmmAllocRsvd(physaddr);
    if (physaddr) {
        print("physAllocRsvd() failed, allocated an available address \n\r");
        rtncde = false;
    } 

    physaddr = pmmAllocRsvd(0x9fc00);
    if (!physaddr) {
        print("physAllocRsvd(0x9fc00) failed, returned 0\n\r");
        rtncde = false;
    } else {
        // not test free
        if (!pmmFreeRsvd(physaddr)) {
            printf("physFree(0x%Xl) failed\n\r", physaddr);
            rtncde = false;
        }
    }

    // try to allcate non reserved memory
    physaddr = pmmAllocRsvd(0x9fc00);
    if (!physaddr) {
        print("physAllocRsvd(0x9fc00) failed, returned 0\n\r");
        rtncde = false;
    } else {
        // not test free
        if (!pmmFreeRsvd(physaddr)) {
            printf("physFree(0x%Xl) failed\n\r", physaddr);
            rtncde = false;
        }
    }

    // test allocate reserved range
    // bad range
    range = (RsvdMemRange_t){123,456};
    if (pmmAllocRsvdRange(range) != MEM_RANGE_ERROR) {
        print("physAllocRsvdRange(badrange) succedded, should have failed\n\r");
        rtncde = false;
    }
    // try a good range
    range = pmmGetRsvdMemRange(0);
    if (pmmAllocRsvdRange(range) == MEM_RANGE_ERROR) {
        print("physAllocRsvdRange(goodrange) failed\n\r");
        rtncde = false;
    } else {
        // deallocate it
        if (!pmmFreeRsvdRange(range)) {
            print("physFreeRsvdRange(valid range) failed\n\r");
            rtncde = false;
        }
    }

    return rtncde;
}