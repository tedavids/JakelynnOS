// tests for virtual memory manager

#include <stdio.h>

#include <vmm.h>

// main tests
bool vmmtests() {

    bool rtncde = true;
    // test the simple stuff first



    // test virtAddrFromPdePdt(uint32_t pde, uint32_t pte)
    // check parameters
    if (AddrFromPdePte(1024,0) != (MemAddr_t) MEM_ADDRESS_ERROR) {
        print("AddrFromPdePte(1024,0) allowed a out of range pde\n\r");
        rtncde = false;
    }

   if (AddrFromPdePte(0,1024) != (MemAddr_t) MEM_ADDRESS_ERROR) {
        print("AddrFromPdePte(0,1024) allowed a out of range pde\n\r");
        rtncde = false;
    }

    // check it works on page 0
    pde_t pde = 0;
    pte_t pte = 0;
    if (AddrFromPdePte(pde,pte) != (MemAddr_t) 0) {
        print("AddrFromPdePte(0,0) did not return address 0x0\n\r");
        rtncde = false;
    }

    pde = 1023;
    pte = 1023;
    if (AddrFromPdePte(pde,pte) != (MemAddr_t) 0xFFFFF000) {
        printf("AddrFromPdePte(1023,1023) did not return address 0xFFFFF000, returned 0x%Xl\n\r",AddrFromPdePte(pde,pte));
        rtncde = false;
    }    
    
    // test PteFromAddr()
    if (PdeFromAddress(0x0) != 0) {
        printf("PdeFromAddress(0x0) failed returned %ul\n\r", PdeFromAddress(0));
        rtncde = false;
    }

    if (PdeFromAddress(0xC0000000) != 768) {
        printf("PdeFromAddress(0xC0000000) failed returned %ul, expected 768\n\r", PdeFromAddress(0));
        rtncde = false;
    }

    if (PdeFromAddress(0xFFFFFFFF) != 1023) {
        printf("PdeFromAddress(0xFFFFFFFF) failed returned %ul\n\r", PdeFromAddress(0));
        rtncde = false;
    }

   // test PteFromAddr()
    if (PteFromAddress(0x0) != 0) {
        printf("PteFromAddress(0x0) failed returned %ul\n\r", PteFromAddress(0));
        rtncde = false;
    }

    if (PteFromAddress(0xFFFFFFFF) != 1023) {
        printf("PteFromAddress(0xFFFFFFFF) failed returned %ul\n\r", PteFromAddress(0));
        rtncde = false;
    }

    // test sync nothing should happen
    if (!pgdirValidatePgDir(false)) {
        print("pgdirValidatePgDir(false) indicated a bad page directory\n\r");
        rtncde = false;
    }
/*
    // test init function
    if (!initVMM()) {
        print("initVMM() failed\n\r");
        // if this fails no point in continuing tests
        return false;
    }
*/
    return rtncde;
}