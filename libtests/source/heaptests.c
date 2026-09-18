// tests for heap

#include <stdio.h>
#include <pagealloctree.h>
#include <vmm.h>
#include <heap.h>

// heap tests
bool heaptests() {

    // page allocation tree tests
    bool rtncde = initPageTree();
    uint32_t depth = getPageTreeDepth();
    // check depth is 0
    
    if (depth != 0) {
        printf("Page tree depth not zero after init, returns %ul\n\r",depth);
        rtncde = false;
    }

    uint32_t freelistsize = getPageTreeFreelistSize();
    if (freelistsize != 256) {
        printf("Freelist size 256 after init, returns %ul\n\r",freelistsize);
        rtncde = false;
    }

    uint32_t numalloc = getPageTreeNumAlloc();
    if (numalloc != 0) {
        printf("Page tree num alloc not zero, returns %ul\n\r", numalloc);
        rtncde = false;
    }

    // test single page alloc/dealloc
    auto address = allocPage(1);
    if (!address) {
        printf("allocPage(1) failed.  Returned: 0x%Xl\n\r",address);
        rtncde = false;
    } else {
        // we have it test several functions
        // check depth is 1
        depth = getPageTreeDepth();
        if (depth != 1) {
            printf("Page tree depth not 1 after alloc, returns %ul\n\r",depth);
            rtncde = false;
        }
        freelistsize = getPageTreeFreelistSize();
        if (freelistsize != 255) {
            printf("Freelist size 255 after alloc, returns %ul\n\r",freelistsize);
            rtncde = false;
        }
        numalloc = getPageTreeNumAlloc();
        if (numalloc != 1) {
            printf("Page tree num alloc not 1 after allocation, returns %ul\n\r", numalloc);
            rtncde = false;
        }
        // check if page is in use
        if (!isVirtMemInUse((uint32_t) address)) {
            print("allocPage() (1) did not set mem in use flag\n\r");
            rtncde = false;
        }
        if (!freePage(address)) {
            printf("freePage(0x%Xl) for 1 page failed\n\r", address);
            rtncde = false;
        }
    }

    // test multiple page allocation
    address = allocPage(5);
    if (!address) {
        printf("allocPage(5) failed, returned 0x%Xl\n\r");
        rtncde = false;
    } else {
        // check if page is in use
        if (!isVirtMemInUse((uint32_t) address)) {
            print("allocPage() (5) did not set mem in use flag\n\r");
            rtncde = false;
        }
        if (!freePage(address)) {
            printf("freePage(0x%Xl) for 5 pages failed\n\r",address);
            rtncde = false;
        }
        // check depth is 1
        depth = getPageTreeDepth();
        if (depth != 1) {
            printf("Page tree depth not 1 after alloc, returns %ul\n\r",depth);
            rtncde = false;
        }
        freelistsize = getPageTreeFreelistSize();
        if (freelistsize != 255) {
            printf("Freelist size 255 after alloc, returns %ul\n\r",freelistsize);
            rtncde = false;
        }
        numalloc = getPageTreeNumAlloc();
        if (numalloc != 1) {
            printf("Page tree num alloc not 1 after allocation, returns %ul\n\r", numalloc);
            rtncde = false;
        }

        if (!freePage(address)) {
            printf("freePage(0x%Xl) for 1 page failed\n\r", address);
            rtncde = false;
        }
    }
    return rtncde;

}