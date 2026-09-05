// multiboot tests

#include <stdio.h>
#include <string.h>

#include <multiboot.h>

bool multiboottest() {
    bool rtncde = true;

    if (!MULTIBOOT_INFO_ADDRESS) {
        print("MULTIBOOT_INFO_ADDRESS is null\n\r");
        return false;
    }

    if (strlen(multiboot_info.cmdline) >= (int)sizeof(multiboot_info.cmdline)) {
        print("Multiboot command line did not load or overflowed\n\r");
        rtncde = false;
    }
    if (strlen(multiboot_info.bootloadername) >= (int)sizeof(multiboot_info.bootloadername)) {
        print("Multiboot bootloadername did not load or overflowed\n\r");
        rtncde = false;
    }
    if (strlen(multiboot_info.modules.string) >= (int)sizeof(multiboot_info.modules.string)) {
        print("Multiboot modules did not load or overflowed\n\r");
        rtncde = false;
    }
    if (multiboot_info.meminfo.lower == 0) {
        print("Multiboot meminfo.lower did not load\n\r");
        rtncde = false;
    }
    if (multiboot_info.meminfo.upper == 0) {
        print("Multiboot meminfo.upper did not load\n\r");
        rtncde = false;
    }
    if (multiboot_info.bootdev.device == 0) {
        print("Multiboot bootdev did not load or is 0\n\r");
        rtncde = false;
    }
    if (multiboot_info.mmap.count == 0) {
        print("Multiboot memory map not loaded\n\r");
        rtncde = false;
    }
    if (multiboot_info.framebuf.phys_address == 0) {
        print("Multiboot frame buffer did not load\n\r");
        rtncde = false;
    }
    if (multiboot_info.ACPI_RSDP.Address == 0) {
        print("Multiboot ACPI info did not load\n\r");
        rtncde = false;
    }
    return rtncde;
}