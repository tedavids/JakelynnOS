// print the memory map, this is part of the shell

#include <stdio.h>
#include <string.h>
#include <multiboot.h>

#include <printmultiboot.h>

// print the memory map
// Parameters:  None

// Returns:     0 on normal completion

int printpmmap() {

    uint64_t meminmegs = multiboot_info.meminfo.upper / 0x100000;

    printf("Physical Memory Map.\n\rAvailable memory: 0x%xl (%uqM) Base Physical Address: 0x%Xl\n\r\n\r", 
        multiboot_info.meminfo.upper, meminmegs, multiboot_info.basephyaddr);

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

// print modules
// Parameters:  None
// Returns:     0 on normal completion, 1 if no modules to print
int printmultibootmodules() {
    if (strlen(multiboot_info.modules.string) == 0) {
        printf("No module information to display\n\r");
        return 1;
    }

    print("Multiboot module information\n\r\n\r");
    printf("Modules loaded: %s\n\r\n\r",multiboot_info.modules.string);

    return 0;
}

// print the boot info
// Parameters:  None
// Returns:     0 on normal completion, 1 if no modules to print
int printbootinfo() {
    print("Boot Information\n\r");

    // device info
    printf("Boot Device: %ul",
        multiboot_info.bootdev.device);
    if (multiboot_info.bootdev.partition != 0xFFFFFFFF) {
        printf(" Partition: 0x%xl",multiboot_info.bootdev.partition);
        if (multiboot_info.bootdev.subpartition != 0xFFFFFFFF) {
            printf(" Sub-partition: 0x%xl",multiboot_info.bootdev.subpartition);
        }
    }
    print("\n\r");

    //boot loader name
    printf("Bootloader: %s\n\r",multiboot_info.bootloadername);

    return 0;
}