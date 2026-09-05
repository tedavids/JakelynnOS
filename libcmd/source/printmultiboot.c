// print the memory map, this is part of the shell

#include <stdio.h>
#include <multiboot.h>

#include <printmultiboot.h>

// print the memory map
// Parameters:  None

// Returns:     0 on normal completion

int printpmmap() {

    uint64_t meminmegs = multiboot_info.meminfo.upper / 0x100000;

    printf("Physical Memory Map.  Available memory: 0x%xl (%uqM)\n\r", 
        multiboot_info.meminfo.upper, meminmegs);

    for (uint32_t i = 0; i < multiboot_info.mmap.count; i++) {
        printf("%ul: Type: %ul Begin: 0x%Xq    End: 0x%Xq\n\r",
            i, multiboot_info.mmap.region[i].memtype, 
            multiboot_info.mmap.region[i].baseaddr,
            multiboot_info.mmap.region[i].endaddr);
    }

    return 0;
}

// print the command line
// Parameters:  None

// Returns:     0 on normal completion
int printcmdline() {

    printf("Kernel command line: %s\n\r",multiboot_info.cmdline);

    return 0;
}