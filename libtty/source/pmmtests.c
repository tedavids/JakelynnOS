// tests for physical memory

#include <stdio.h>

#include <multiboot.h>
#include <pmm.h>

// include externals we need
extern uint32_t _kernel_start;
extern uint32_t _heap_start;

// variables
uint32_t testaddress = (uint32_t) &_heap_start;
// test main 

bool pmmtest() {
    bool rtncde = true;

    // test base functions
    if (AddressToPage(0x1234567) != 0x1234) {
        printf("AddressToPage(0x1234567) failed, returnd: 0x%Xl\n\r",AddressToPage(0x1234567));
        return false;
    }
    // test addrFromPDEPDT
    if (physAddrFromPdePdt(768,1) != 0xC0001000) {
        printf("addrFromPdePdt(768,1) did not return 0xC0001000, returned: 0x%Xl\n\r",physAddrFromPdePdt(768,1));
        return false;
    }

    // test the arrays
    uint32_t existcount = 0;
    uint32_t availcount = 0;
    uint32_t reservedcount = 0;
    uint32_t inuse = 0;
    // test physical memory arrays
    for (uint32_t page = 0; page <= 0xFFFFF; page++) {
        if (physMemExists(page)) existcount++;
        if (physMemAvail(page)) availcount++;
        if (physMemReserved(page)) reservedcount++;
        // we have no shared pages to worry about yet
        if (physMemInUse(page)) inuse++;
    }
    
    // does exist count match
    if (existcount != PhysMemInfo.PagesExist) {
        printf("doesPhysMemExist returns %ul, PhysMemInfo.PagesExist = %ul\n\r", 
            existcount, PhysMemInfo.PagesExist);
        rtncde = false;
    }
    // does availcount match
    if (availcount != PhysMemInfo.PagesAvail) {
        printf("isPhysMemAvail returns %ul, PhysMemInfo.PagesAvail = %ul\n\r", availcount, PhysMemInfo.PagesAvail);
        rtncde = false;
    }
    // does reserved count match
    if (reservedcount != PhysMemInfo.PagesReserved) {
        printf("isPhysMemReserved returns %ul, PhysMemInfo.PagesReserved = %ul\r\n",reservedcount,PhysMemInfo.PagesReserved);
        rtncde = false;
    }
    // does in use count mathc
    if (inuse != PhysMemInfo.PagesInUse) {
        printf("isPhysMemInUse returns %ul, PhysMemInfo.PagesInUse = %ul\r\n", inuse, PhysMemInfo.PagesInUse);
        rtncde = false;
    }
    // test memory available/reserved array

    // test in use array

    // test functions

    // memory exists
    if (!physMemExists(AddressToPage(0x0))) {
        printf("Error:  Function: doesPhysMemExist(0x0) retured false\n\r");
        rtncde = false;
    }
    if (physMemExists(0xAFFFF)) {
        printf("Error:  Function: doesPhysMemExist(0xAFFFF) returnd true\n\r");
        rtncde = false;
    }


    return rtncde;
}