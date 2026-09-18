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
typedef struct{
    uint32_t    start;
    uint32_t    end;
    char        string[512];
} multiboot_modules_t ;

// tag 4
typedef struct {
    uint32_t    lower;      // first MULTIBOOT_INFO_CONFIG_TABLE
    uint32_t    upper;      // actual memory size
} multiboot_memory_info_t;

// tag 5
typedef struct {
    uint32_t    device;         // device ID
    uint32_t    partition;      // partition
    uint32_t    subpartition;   // sub-partison 
} multiboot_bios_dev_info_t;

// tag 6 individual entries
typedef struct {
    uint64_t    baseaddr;       // base address
    uint64_t    endaddr;        // end of the block
    uint32_t    memtype;        // type of memory 1 = available, 3 = ACPI, 5 is defective
                                // type 1 is read/write, type 5 should be set to unavailable
                                // all others should be read only
    uint32_t    reserved;
} multiboot_mem_map_entry_t;

// tag 6
typedef struct {
    uint32_t                            count;          // number of entries
    uint32_t                            version;        // version for entry
    multiboot_mem_map_entry_t           region[MULTIBOOT_MAX_REGIONS];     // if we have more than 10 decide what TODO
} multiboot_mem_map_info_t;

// tag 8 framebuffer palette
typedef struct {
    uint8_t     red;
    uint8_t     green;
    uint8_t     blue;
} multiboot_framebuffer_palette_entry_t;

// tag 8 frame buffer palette info
typedef struct {
    uint32_t    numColors;
    multiboot_framebuffer_palette_entry_t   palette[256];
} multiboot_framebuffer_indexed_color_t;

// tag 8 framebuffer rgb colors
typedef struct {
    uint8_t     redPosition;
    uint8_t     redMaskSize;
    uint8_t     greenPosition;
    uint8_t     greenMaskSize;
    uint8_t     bluePosition;
    uint8_t     blueMaskSize;
} multiboot_framebuffer_RGB_info_t;

// tag 8
typedef struct {
    uint64_t    phys_address;
    uint32_t    pitch;
    uint32_t    width;
    uint32_t    height;
    uint8_t     bitsPerPixel;
    uint8_t     colorType;
    union {
        multiboot_framebuffer_indexed_color_t    indexedColor;
        multiboot_framebuffer_RGB_info_t         rgbColor;
    } palette;
} multiboot_framebuffer_info_t;

// tag 14 or 15 ACPI RSDP info
typedef struct {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t Length;
    uint64_t Address;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} multiboot_ACPI_RSDP_t;

// tag 10 APM Info
typedef struct {
    uint16_t        version;
    uint32_t        offset;
    uint16_t        cseg;
    uint16_t        dseg;
    uint16_t        flags;
    uint16_t        cseg_len;
    uint16_t        dseg_len;
} multiboot_APM_t;

// my structure to hold ALL multiboot info
typedef struct {
    char                                cmdline[1028];
    char                                bootloadername[256];
    multiboot_modules_t                 modules;
    multiboot_memory_info_t             meminfo;
    multiboot_bios_dev_info_t           bootdev;
    multiboot_mem_map_info_t            mmap;
    multiboot_framebuffer_info_t        framebuf;
    multiboot_ACPI_RSDP_t               ACPI_RSDP;
    uint32_t                            basephyaddr;
    multiboot_APM_t                     APM;
} multiboot_info_t;


extern struct multiboot_info_header_t *MULTIBOOT_INFO_ADDRESS;
extern multiboot_info_t  multiboot_info;

extern bool loadMultibootInfo();
#endif