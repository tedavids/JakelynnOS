// this will be my kernel

/* Copyright (C) <year> <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// remove this when you dont want to test
#include <tests.h>
#include <stdint.h>
#include <stdbool.h>
#include <tty.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <kbd.h>
#include <kernel>
#include <krnlcmdline.h>
#include <pmm.h>
#include <vmm.h>
//#include <paging.h>
#include <idt.h>
#include <time.h>
#include <interrupt.h>
#include <multiboot.h>
#include <ps2kbd.h>
#include <docmd.h>
#include <heap.h>


#include "kernel.h"

// The start of the kernel

// Parameters:  None

// Returns:     None -- this is not meant to return

void kernel_main() {

   // init display 
    if (!ttyInit()) {
        printf("Problem initializing terminal\r\n");
        abort();
    }

    //  print name they can read while I do other stuff
    printf("%s Boot Version: %s Date: %s\n\r",(char *)&BOOTNAME,(char *)&BOOTVERSION,(char *)&BOOTDATE);
    print("JakelynnOS Version: 0.0.0 Date: 2026-09-08\n\r");
    // system clock
    rtime_t time = read_system_clock();
    printf("Time: %uw/%ub/%ub %ub:%ub:%ub\n\r",
        time.year, time.month, time.day, time.hour, time.minute, time.second);

    printf("Boot Status: 0x%xl\r\n",BOOTSTATUS);


    // load the IDT, this MUST follow load APIC because it starts
    // interrupts again
    printf("Loading IDT...");

    if (!loadIDT()) {
        printf("FAILED!\n\r");
        abort();
    } 
    printf("Success\n\r");

        // APIC timer
    printf("Setting up APIC Timer...");
    if (!initAPICtimer()) {
        printf("Failed\n\r");
        abort();
    } 
    printf("Success\n\r");

    // turn on interrupts, everything is set up
    printf("Turning on interrupts...");
    EnableInterrupts();
    printf("Success\n\r");
    

    // Multiboot info
    printf("Loading multiboot info...");
    if (!loadMultibootInfo()) {
        printf("Failed\n\r");
        abort();
    }
    printf("Success\n\r");

    // processing command line
    printf("Processing kernel command line...");
    if (!processCommandLine(multiboot_info.cmdline)) {
        printf("Failed\n\r");
        abort();
    }
    printf("Success\n\r");

    // setting up final page directory and tables
    printf("Setting up page tables...");
    if (!initPageDirectory()) {
        printf("Failed\n\r");
        abort();
    }
    printf("Success\n\r");

    #if defined(INCLUDE_TESTS) && defined(PMM_TESTS)
    print("Starting Physical memory manager tests...");
    if (!pmmtests()) {
        printf("Failed\n\r");
    } else {
        printf("Successful\n\r");
    }
#endif

    // initialize memory managers
    printf("Initializing physical memory manager...");
    if (!initPMM(&multiboot_info.meminfo, &multiboot_info.mmap)) {
        printf("Failed\n\r");
        abort();
    }
    printf("Success\n\r");

#if defined(INCLUDE_TESTS) && defined(VMM_TESTS)
    print("Starting virtual memory manager tests...\r\n");
    if (!vmmtests()) {
        printf("Failed\n\r");
    } else {
        printf("Successful\n\r");
    }
#endif

    printf("Initializing virtual memory manager...");
    if (!initVMM()) {
        printf("Failed\n\r");
        abort();
    }
    printf("Success\n\r");   
/*
    // initialize paging
    printf("Initializing paging...");
    if (!initPaging()) {
        printf("Failed\n\r");
        abort();
    }
    printf("Success\n\r");


    // initialize heap
    printf("Initializing heap...");
    if (!initHeap()) {
        printf("Failed\n\r");
        abort();
    } else {
        printf("Success\n\r");
    }
*/
    // initialize keyboard
    printf("Initializing keyboard...");
    if (initKbd()) {
        printf("Success\n\r");
    } else {
        printf("Failed\n\4");
        abort();
    }
        // command loop
    char command[256];

    int rtncde = 0;
    while (strncmp(sizeof(command), command,"exit") !=0 ) {
        printf("Command> ");
        if (kbdInput(command,sizeof(command),true)) {
            printf("\r");
            rtncde = doCommand(command);
            printf("Return code: %iw\n\r", rtncde);
        }

    }

    print("Kernel end");

    // this is the end of the kernel

    while (true);

}
