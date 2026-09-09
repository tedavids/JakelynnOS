// tests for virtual memory manager

#include <stdio.h>

#include <vmm.h>

// main tests
bool vmmtests() {

    bool rtncde = true;
    // test the simple stuff first

    pde_t pde;
    pte_t pte;

    // test virtAddrFromPdePdt(uint32_t pde, uint32_t pte)
    // check parameters
    if (virtAddrFromPdePdt(1024,0) != 0xFFFFFFFF) {
        print("virtAddrFromPdePdt(1024,0) allowed a out of range pde\n\r");
        rtncde = false;
    }

   if (virtAddrFromPdePdt(0,1024) != 0xFFFFFFFF) {
        print("virtAddrFromPdePdt(0,1024) allowed a out of range pde\n\r");
        rtncde = false;
    }


    // check it works on page 0
    pde = 0;
    pte = 0;
    if (virtAddrFromPdePdt(pde,pte) != 0) {
        print("virtAddrFromPdePdt(0,0) did not return address 0x0\n\r");
        rtncde = false;
    }

    pde = 1023;
    pte = 1023;
    if (virtAddrFromPdePdt(pde,pte) != 0xFFFFF000) {
        printf("virtAddrFromPdePdt(1023,1023) did not return address 0xFFFFF000\n\r" );
        rtncde = false;
    }    
    
    // test init function
    if (!initVMM()) {
        print("initVMM() failed\n\r");
        // if this fails no point in continuing tests
        return false;
    }

    // test tghe simple functions
    // is it kernel memory
    if (isVirtKrnlMem(0x0)) {
        print("isVirtKrnlMem(0x0) returned true, failed\n\r");
        rtncde = false;
    }

    if (!isVirtKrnlMem(0xC0000001)) {
        print("isVirtKrnlMem(0xC0000001) returned true, failed\n\r");
        rtncde = false;
    }

    // test read only
    // not read only
    if (isVirtMemReadOnly(0x0)) {
        print("isVirtMemReadonly(0x0) returned true, failed\n\r");
        rtncde = false;
    }

    // read only
    if (!isVirtMemReadOnly(0xc0101000)) {
        print("isVirtMemReadonly(0xc0101000) returned false, failed\n\r");
        rtncde = false;
    }

    // test the set function
    if (!setVirtMemReadOnly(0x0)) {
        // 0x0 should be read only
        if (!isVirtMemReadOnly(0x0)) {
            print("setVirtMemReadOnly(0x0) failed\n\r");
            rtncde = false;
        } else {
            // since its now read only, make sure it still in use
            if (!isVirtMemInUse(0x0)) {
                print("problem with setVirtMemReadOnly(0x0), no longer in use\n\r");
                rtncde = false;
            } else {
                // try to change it back
                if (!clearVirtMemReadOnly(0x0)) {
                    printf("Problem with clearVirtualMemReadOnly(0x0), returned false\n\r");
                    rtncde = false;
                } else {
                    if (isVirtMemReadOnly(0x0)) {
                        printf("clearVirtMemReadOnly(0x0) failed\n\r");
                        rtncde = false;
                    }
                }
            }
        }
    }

    // test swappable
    // not swappable
    if (isVirtMemSwappable(0x0)) {
        print("isVirtMemSwappable(0x0) returned true, failed\n\r");
        rtncde = false;
    }

    // set it swappable
    if (!setVirtMemSwappable(0)) {
        print("setVirtMemSwappable(0) failed\n\r");
        return false;
    } else {
        if (!isVirtMemSwappable(0)) {
            printf("setVirtMemSwappable(0) failed on confirmation\n\r");
            rtncde = false;
        }
    }

    // swappable
    if (clearVirtMemSwappable(0x0)) {
        if (isVirtMemSwappable(0x0)) {
            printf("clearVirtMemSwappable(0x0) failed on confirmation");
            rtncde = false;
        }
    } else {
        printf("clearVirtMemSwappable(0x0) failed\n\r");
        rtncde = false;
    }

    // is it swapped out
    // these tests need a swappable address
    setVirtMemSwappable(0x0);

    // nothing is swapped out to start
    if (isVirtMemSwappedOut(0x0)) {
        print("isVirtMemSwappedOut(0x0) failed, returned true\n\r");
        rtncde = false;
    }

    // swap it out
    if (!setVirtMemSwappedOut(0x0)) {
        print("setVirtMemSwappedOut(0x0) failed\n\r");
        rtncde = false;
    } else {
        // does it show swapped out
        if (!isVirtMemSwappedOut(0x0)) {
            print("setVirtMemSwappedOut(0x0) failed on confirmation\n\r");
            rtncde = false;
        } 
    }

    // swap it back in
    if (!clearVirtMemSwappedOut(0x0)) {
        print("clearVirtMemSwappedOut(0x0) failed\n\r");
        rtncde = false;
    } else {
        if (isVirtMemSwappedOut(0x0)) {
            print("clearVirtMemSwappedOut(0x0) failed on confirmation\n\r");
            rtncde = false;
        }
    }
    // clear the swappable address
    clearVirtMemSwappedOut(0x0);

    // virtual memory in use
    
    // test if it is in use
    if (!isVirtMemInUse(0x0)) {
        printf("isVirtMemInUse(0x0) failed\n\r");
        rtncde = false;
    }

    if (isVirtMemInUse((0xA0000))) {
        printf("isVirtMemInUse(0xA0000) failed\n\r");
        rtncde = false;
    }

    // test clear since it is already in use
    if (!clearVirtMemInUse(0x0)) {
        print("clearVirtMemInUse(0x0) failed\n\r");
        rtncde = false;
    } else {
        if (isVirtMemInUse(0x0)) {
            print("clearVirtMemInUse(0x0) failed on confirmation\n\r");
            rtncde = false;
        }
    }

    // test if it gets set back
    if (!setVirtMemInUse(0x0)) {
        print("setVirtMemInUse(0x0) failed\n\r");
        rtncde = false;
    } else {
        if (!isVirtMemInUse(0x0)) {
            print("isVirtMemInUse(0x0) failed on confirmation\n\r");
            rtncde = false;
        }
    }

    // test kernel memory
    if (!isVirtKrnlMem(0xC0200000)) {
        print("isVirtKrnlMem(0xC0200000) failed\n\r");
        rtncde = false;
    }

    if (isVirtKrnlMem(0x0)) {
        print("isVirtKrnlMem(0x0) failed, returned true\n\r");
        rtncde = false;
    }

    // set kernel memory
    if (!setVirtKrnlMem(0x0)) {
        print("setVirtKrnlMem(0x0) failed\n\r");
        rtncde = false;
    } else {
        if (!isVirtKrnlMem(0x0)) {
            print("setVirtKrnlMem(0x0) failed on validation\n\r");
            rtncde = false;
        }
    }

    // clear it
    if (!clearVirtKrnlMem(0x0)) {
        print("clearVirtKrnlMem(0x0) failed");
        rtncde = false;
    } else {
        if (isVirtKrnlMem(0x0)) {
            print("clearVirtKrnlMem(0x0) failed on validation\n\r");
            rtncde = false;
        }
    }

    // test shared memory
    // 0x0 is unshared we know that
    if (isVirtMemShared(0x0)) {
        print("isVirtMemShared(0x0) failed\n\r");
        rtncde = false;
    }

    // set it shared
    if (!setVirtMemShared(0x0)) {
        print("setVirtMemShared(0x0) failed\n\r");
    } else {
        if (!isVirtMemShared(0x0)) {
            print("isVirtMemShared(0x0) failed on validation\n\r");
            rtncde = false;
        }
    }

    // unshare
    if (!clearVirtMemShared(0x0)) {
        print("clearVirtMemShared(0x0) failed\n\r");
        rtncde = false;
    } else {
        if (isVirtMemShared(0x0)) {
            print("clearVirtMemShared(0x0) failed on validation\n\r");
            rtncde = false;
        }
    }


    // get next available kernel page
    auto firstpage = getNextAvailKernelPage();
    
    // this should be the next page
    auto page = getNextAvailKernelPage();
    if (page != (firstpage + 1)) {
        printf("getNextAvailKernelPage failed, returned 0x%xl, should be 0x%xl\n\r",
            page, (firstpage + 1));
        rtncde = false;
    }
    
    // test allocation
    auto addr = allocVirtKrnlMem(true);
    if (!addr) {
        print("allocVirtKrnlMem(true) failed\n\r");
        rtncde = false;
    } else {
        // test memory flags
        if (!isVirtMemInUse((uint32_t)addr)) {
            print("allocVirtKrnlMem(true) corrupted the in use flag\n\r");
            rtncde = false;
        }
        if (!isVirtKrnlMem((uint32_t)addr)) {
            print("allocVirtKrnlMem(true) corrupted the kernel memory flag\n\r");
            rtncde = false;
        }
        if (isVirtMemReadOnly((uint32_t)addr)) {
            print("allocVirtKrnlMem(true) corrupted the read only flag\n\r");
            rtncde = false;
        }
        if (isVirtMemShared((uint32_t)addr)) {
            print("allocVirtualKrnlMem(true) corrupted the shared flag\n\r");
            rtncde = false;
        }
        if (!isVirtMemSwappable((uint32_t)addr)) {
            printf("allocVirtKrnlMem(true) corrupted teh swappable flag\n\r");
            rtncde = false;
        }
        if (isVirtMemSwappedOut((uint32_t)addr)) {
            printf("allocVirtKrnlMem(true) corrupte the swapped out flag\n\r");
            rtncde = false;
        }
    }

    // test that the page is good
    print("Testing allocation, is page good...");
    *addr = 5;
    print("success\n\r");

    // test deallocation
    if (!deallocVirtKrnlMem((uint32_t)addr, true)) {
        print("deAllocVirtKrnlMem(true) failed\n\r");
        rtncde = false;
    } else {
        // test memory flags
        if (isVirtMemInUse((uint32_t)addr)) {
            print("deallocVirtKrnlMem(true) corrupted the in use flag\n\r");
            rtncde = false;
        }
        if (!isVirtKrnlMem((uint32_t)addr)) {
            print("deallocVirtKrnlMem(true) corrupted the kernel memory flag\n\r");
            rtncde = false;
        }
        if (isVirtMemReadOnly((uint32_t)addr)) {
            print("deallocVirtKrnlMem(true) corrupted the read only flag\n\r");
            rtncde = false;
        }
    }

    // test our routine to get a range of kernel pages

    address_range_t range = getAvailKernelPageRange(5);

    if ((range.lowpage == 0xFFFFFFFF) && (range.highpage == 0xFFFFFFFF)) {
        printf("getAvailKernelPageRange(5) failed\n\r");
        rtncde = false;
    } else {
        if ((range.highpage - range.lowpage) != 4) {
            printf("getAvailKernelPageRange(5) did not return the proper range (%Xl,%Xl)\n\r", range.lowpage, range.highpage);
            rtncde = false;
        } else {
            // we can now test our allocation function
            uint32_t lastallocated;
            uint32_t *addr = allocVirtMemBlock(range.lowpage<<12,range.highpage<<12,&lastallocated);
            if (!addr) {
                printf("allocVirtMemBlock failed\n\r");
                rtncde = false;
            } else {
                if (lastallocated != (range.highpage<<12)) {
                    printf("allocVirtMemBlock failed.  highpage = %xl, last alloc = %xl\r\n", range.highpage, lastallocated);
                    rtncde = false;
                }
                // test dealloc
                bool status = deallocVirtMemBlock(range.lowpage<<12,range.highpage<<12, &lastallocated);
                if (!status) {
                    printf("deallocVirtMemBlock(%Xl,%Xl,lastalloc) failed, last dealloc: %Xl\n\r",
                        range.lowpage<<12, range.highpage<<12,lastallocated);
                    rtncde = false;
                }
            }
        }
    }

    return rtncde;
}