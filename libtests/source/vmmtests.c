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

    // is valid index
    if (pgdirIsValidIdx(1024)) {
        print("pgdirIsValidIdx(1024) returned true\n\r");
        rtncde = false;
    }
    if (!pgdirIsValidIdx(0)) {
        print("pgdirIsValidIdx(0) failed\n\r");
        rtncde = false;
    }
    if (!pgdirIsValidIdx(1023)) {
        print("pgdirIsValidIdx(1023) failed\n\r");
        rtncde = false;
    }

    // test init function
    if (!initVMM()) {
        print("initVMM() failed\n\r");
        // if this fails no point in continuing tests
        return false;
    }

    // page directory tests

    // test various flags, since we are in a known state
    if (!pgdirValidatePgDir(false)) {
        print("pgdirValidatePgDir(false) failed\n\r");
        rtncde = false;
    }

    // these should return false for now
    if (pgdirIsPAT(0)) {
        print("pgdirIsPAT(0) returned true\n\r");
        rtncde = false;
    }
    if (pgdirIsPATMem(0)) {
        print("pgdirIsPATMem(0) returned true\n\r");
        rtncde = false;
    }
    if (pgdirIsCacheDisabled(0)) {
        print("pgdirIsCacheDisabled(0) returned true\n\r");
        rtncde = false;
    }
    if (pgdirIsCacheDisabledMem(0)) {
        print("pgdirIsCacheDisabledMem(0) returned true\n\r");
        rtncde = false;
    }
    if (pgdirIsWriteThru(0)) {
        print("pgdirIsWriteThru(0) returned true\n\r");
        rtncde = false;
    }
    if (pgdirIsWriteThruMem(0)) {
        print("pgdirIsWriteThruMem(0) returned true\n\r");
        rtncde = false;
    }
    
    // test global
    if (pgdirIsGlobal(0)) {
        print("pgdirIsGlobal(0) returned true\n\r");
        rtncde = false;
    }
 
    // set it true and try again
    if (!pgdirSetGlobal(0)) {
        print("pgdirSetGlobal(0) failed\n\r");
        rtncde = false;
    } else {
        if (!pgdirIsGlobal(0)) {
            print("pgdirSetGlobal(0) failed when checked\n\r");
            rtncde = false;
        } else {
            // test changing it back
            if (!pgdirClearGlobal(0)) {
                print("pgdirClearGlobal(0) failed\n\r");
                rtncde = false;
            } else {
                // check it cleared
                if (pgdirIsGlobal(0)) {
                    print("pgdirClearGlobal(0) failed on check\n\r");
                    rtncde = false;
                }
            }
        }
    }

   // test global mem in the actual page directory
   if (pgdirIsGlobalMem(0)) {
        print("pgdirIsGlobalMem(0) returned true\n\r");
        rtncde = false;
    } {
        // test set and clear
        if (!pgdirChgGlobalMem(0,true, false)) {
            print("pgdirChgGlobalMem(0,true,false) failed\n\r");
            rtncde = false;
        } else {
            // make sure it changed
            if (!pgdirIsGlobalMem(0)) {
                print("pgdirChgGlobalMem(0,true,false) failed on check\n\r");
                rtncde = false;
            } else {
                // change it back
                if (!pgdirChgGlobalMem(0, false, false)) {
                    print("pgdirChgGlobalMem(0,false,false) failed\n\r");
                    rtncde = false;
                } else {
                    // check it really changed
                    if (pgdirIsGlobalMem(0)) {
                        print("pgdirChgGlobalMem(0,false,false) failed on check\n\r");
                        rtncde = false;
                    }
                }
            }
        }
    }
    

    // test 4k flag
    if (pgdirIs4M(0)) {
        print("pgdirIs4M(0) returned true\n\r");
        rtncde = false;
    }

    // set it
    if (!pgdirSet4M(0)) {
        print("pgdirSet4M(0) failed\n\4");
        rtncde = false;
    } else {
        //make sure it changed
        if (!pgdirIs4M(0)) {
            print("pgdirSet4M(0) failed on validation\n\r");
            rtncde = false;
        } else {
            // change it back
            if (!pgdirClear4M(0)) {
                print("pgdirClear4M(0) failed\n\r");
                rtncde = false;
            } else {
                // make sure it changed
                if (pgdirIs4M(0)) {
                    print("pgdirClear4M(0) failed on validation\n\r");
                    rtncde = false;
                }
            }
        }
    }

    // test changing in memory
    if (pgdirIs4MMem(0)) {
        print("pgdirIs4MMem(0) returned true\n\r");
        rtncde = false;
    } {
        // test set and clear
        if (!pgdirChg4MMem(0,true, false)) {
            print("pgdirChg4MMem(0,true,false) failed\n\r");
            rtncde = false;
        } else {
            // make sure it changed
            if (!pgdirIs4MMem(0)) {
                print("pgdirChg4MMem(0,true,false) failed on check\n\r");
                rtncde = false;
            } else {
                // change it back
                if (!pgdirChg4MMem(0, false, false)) {
                    print("pgdirChg4MMem(0,false,false) failed\n\r");
                    rtncde = false;
                } else {
                    // check it really changed
                    if (pgdirIs4MMem(0)) {
                        print("pgdirChg4MMem(0,false,false) failed on check\n\r");
                        rtncde = false;
                    }
                }
            }
        }
    }

    // test user page property flags
    if (pgdirIsUserPage(0)) {
        print("pgdirIsUserPage(0) failed\n\r");
        rtncde = false;
    } else {
        // test changing it
        if (!pgdirSetKernelPage(0)) {
            print("pgdirSetKernelPage(0) failed\n\r");
            rtncde = false;
        } else {
            // verify it
            if (!pgdirIsKernelPage(0)) {
                print("pgdirSetKernelPage(0) failed on verify\n\r");
                rtncde = false;
            } else {
                // change it back
                if (!pgdirSetUserPage(0)) {
                    printf("pgdirSetUserPage(0) failed/n/r");
                    rtncde = false;
                }
            }
        }
    }
    if (pgdirIsKernelPage(0)) {
        print("pgdirIsKernelPage(0) failed\n\r");
        rtncde = false;
    }

    return rtncde;
}