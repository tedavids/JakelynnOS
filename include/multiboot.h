// multibootmemorymap.h

/* Copyright (C) 2026 Tom Davidson

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

#ifndef _MULTIBOOTINFO_H
#define _MULTIBOOTINFO_H

#include <stdint.h>

#define MULTIBOOT_MAX_REGIONS   20

// tag 3
struct multiboot_modules_t {
    uint32_t    start;
    uint32_t    end;
    char        string[512];
};

// tag 4
struct multiboot_memory_info_t {
    uint32_t    lower;      // first MULTIBOOT_INFO_CONFIG_TABLE
    uint32_t    upper;      // actual memory size
};

// tag 5
struct multiboot_bios_dev_info_t {
    uint32_t    device;         // device ID
    uint32_t    partition;      // partition
    uint32_t    subpartition;   // sub-partison 
};

// tag 6 individual entries
struct multiboot_mem_map_entry_t {
    uint64_t    baseaddr;       // base address
    uint64_t    endaddr;        // end of the block
    uint32_t    memtype;           // type of memory 1 = available, 3 = ACPI, 5 is defective
                                // type 1 is read/write, type 5 should be set to unavailable
                                // all others should be read only
    uint32_t    reserved;
};

// tag 6
struct multiboot_mem_map_info_t {
    uint32_t                            count;          // number of entries
    uint32_t                            version;        // version for entry
    struct multiboot_mem_map_entry_t    region[MULTIBOOT_MAX_REGIONS];     // if we have more than 10 decide what TODO
};

// tag 8 framebuffer palette
struct multiboot_framebuffer_palette_entry_t {
    uint8_t     red;
    uint8_t     green;
    uint8_t     blue;
};

// tag 8 frame buffer palette info
struct multiboot_framebuffer_indexed_color_t {
    uint32_t    numColors;
    struct multiboot_framebuffer_palette_entry_t   palette[256];
};

// tag 8 framebuffer rgb colors
struct multiboot_framebuffer_RGB_info_t {
    uint8_t     redPosition;
    uint8_t     redMaskSize;
    uint8_t     greenPosition;
    uint8_t     greenMaskSize;
    uint8_t     bluePosition;
    uint8_t     blueMaskSize;
};

// tag 8
struct multiboot_framebuffer_info_t {
    uint64_t    phys_address;
    uint32_t    pitch;
    uint32_t    width;
    uint32_t    height;
    uint8_t     bitsPerPixel;
    uint8_t     colorType;
    union {
        struct multiboot_framebuffer_indexed_color_t    indexedColor;
        struct multiboot_framebuffer_RGB_info_t         rgbColor;
    } palette;
};

// tag 14 or 15 ACPI RSDP info
struct multiboot_ACPI_RSDP_t {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t Length;
    uint64_t Address;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
};


// my structure to hold ALL multiboot info
struct multiboot_info_t {
    char                                cmdline[1028];
    char                                bootloadername[256];
    struct multiboot_modules_t          modules;
    struct multiboot_memory_info_t      meminfo;
    struct multiboot_bios_dev_info_t    bootdev;
    struct multiboot_mem_map_info_t     mmap;
    struct multiboot_framebuffer_info_t framebuf;
    struct multiboot_ACPI_RSDP_t        ACPI_RSDP;
};


extern struct multiboot_info_header_t *MULTIBOOT_INFO_ADDRESS;
extern struct multiboot_info_t  multiboot_info;

extern bool loadMultibootInfo();
#endif