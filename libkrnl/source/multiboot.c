// multiboot.c Get information from multiboot

#include <stdint.h>
#include <stdbool.h>

#include <kernel>
#include <string.h>
#include <stdio.h>
#include <multiboot.h>

#define END_TAG             0
#define CMDLINE_TAG         1
#define BOOT_LOADER_TAG     2
#define MODULES_TAG         3
#define MEMORY_SIZES_TAG    4
#define BIOS_BOOT_DEV_TAG   5
#define MMAP_TAG            6
#define VBE_TAG             7
#define FRAMEBUFFER_TAG     8
#define POWER_MGMT_TAG      10
#define EFI_SYS_TBL_PTR_TAG 11
#define SMBIOS_TABLE_TAG    13
#define ACPI_OLD_RDSP_TAG   14
#define ACPI_NEW_RDSP_TAG   15
#define NET_INFO_TAG        16
#define EFI_MEMORY_MAP_TAG  17
#define BOOT_SERVICES_ACT_TAG 18
#define IMAGE_LOAD_BASE_TAG 21
#define MMAP_NO_RAM         99

// this structure holds the multiboot informatino

struct multiboot_info_t  multiboot_info;

// local structures
struct tag_t {
    uint32_t type;
    uint32_t size;
};

struct tag_mem_info_t {
    uint32_t    type;
    uint32_t    size;
    uint32_t    lower;
    uint32_t    upper;
};

struct tag_boot_dev_t {
    uint32_t    type;
    uint32_t    size;
    uint32_t    biosdev;
    uint32_t    partition;
    uint32_t    subpartition;
};

struct tag_string_t {
    uint32_t    type;
    uint32_t    size;
    char        string[];
};

struct tag_mmap_entry_t {
    uint64_t    baseaddr;
    uint64_t    len;
    uint32_t    type;
    uint32_t    reserved;
};

struct tag_mmap_t {
    uint32_t            type;
    uint32_t            size;
    uint32_t            entrysize;
    uint32_t            version;
    struct tag_mmap_entry_t    entry[10];
};

struct multiboot_info_header_t {
    uint32_t    size;       // total_size
    uint32_t    reserved;   // reserved
};

struct tag_modules_t {
    uint32_t        type;
    uint32_t        size;
    uint32_t        mod_start;
    uint32_t        mod_end;
    char            string[512];
};

// ACPI RSDP tag 14 (copied from: https://wiki.osdev.org/RSDP)
struct tag_RSDP_t {
    uint32_t    type;
    uint32_t    size;
    char        Signature[8];
    uint8_t     Checksum;
    char        OEMID[6];
    uint8_t     Revision;
    uint32_t    RsdtAddress;
} __attribute__ ((packed));

// ACPI RSDP Tag 15 structure for revision 2 (version 2.0+) (copied from https://wiki.osdev.org/RSDP)
struct tag_XSDP_t {
    uint32_t    type;
    uint32_t    size;
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;      // deprecated since version 2.0
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed));

struct tag_framebuffer_t {
    uint32_t        type;
    uint32_t        size;
    uint64_t        address;
    uint32_t        pitch;
    uint32_t        width;
    uint32_t        height;
    uint8_t         bpp;
    uint8_t         fbtype;
    union {
        struct multiboot_framebuffer_indexed_color_t    indexedColor;
        struct multiboot_framebuffer_RGB_info_t         rgbColor;
    } palette;
};

struct tag_image_base_phy_addr_t {
    uint32_t        type;
    uint32_t        size;   
    uint32_t        basephyaddr;
};

struct tag_APM_t {
    uint32_t        type;
    uint32_t        size;
    uint16_t        version;
    uint16_t        cseg;
    uint32_t        offset;
    uint16_t        cseg_16;
    uint16_t        dseg;
    uint16_t        flags;
    uint16_t        cseg_len;
    uint16_t        cseg16_len;
    uint16_t        dseg_len;
}  __attribute__ ((packed));

// helper function
static uint32_t align8(uint32_t x) {
    return  ((x + 7) & ~(uint32_t)7);
}

// build the memory map

void buildMMAP(const struct tag_mmap_t* const mmap) {
    multiboot_info.mmap.version = mmap->version;
    multiboot_info.mmap.count = (mmap->size - (3 * sizeof(uint32_t))) / 
                                sizeof(struct tag_mmap_entry_t);

    // check that we don't overrun our entries
    if (multiboot_info.mmap.count > 10) {
        printf("WARNING:  More map segments than available space: %ul\n\r",
            multiboot_info.mmap.count);
        multiboot_info.mmap.count = 10;
    }

    // copy segments
    for (int i = 0; i < (int) multiboot_info.mmap.count; i++) {
        multiboot_info.mmap.region[i].baseaddr = mmap->entry[i].baseaddr;
        // memory is 0 based, length is 1 based so subtract 1
        multiboot_info.mmap.region[i].endaddr = mmap->entry[i].baseaddr + 
            mmap->entry[i].len - 1;
        multiboot_info.mmap.region[i].memtype = mmap->entry[i].type;
    }
}

// load multiboot information

bool loadMultibootInfo() {
    if (MULTIBOOT_INFO_ADDRESS == nullptr) return false;

    // we know our address is good
    // get the info header
    struct multiboot_info_header_t *mbi = (struct multiboot_info_header_t*) MULTIBOOT_INFO_ADDRESS;
    struct tag_t *tag = (struct tag_t*) ((uint8_t*)mbi +8);

    // load the tags
    while (tag->type != END_TAG) {
        switch (tag->type) {
            case CMDLINE_TAG: {
                struct tag_string_t *s = (struct tag_string_t *) tag;
                strncpy(multiboot_info.cmdline,s->string, sizeof(multiboot_info.cmdline));

                break;
                }
            case BOOT_LOADER_TAG: {
                struct tag_string_t *s = (struct tag_string_t*) tag;
                strncpy(multiboot_info.bootloadername, s->string, sizeof(multiboot_info.bootloadername));
                break;
                }
            case MODULES_TAG: {
                struct tag_modules_t *s = (struct tag_modules_t *) tag;
                multiboot_info.modules.start = s->mod_start;
                multiboot_info.modules.end = s->mod_end;
                strncpy(multiboot_info.modules.string, s->string, sizeof(multiboot_info.modules.string));

                break;
            }
            case MEMORY_SIZES_TAG: {
                struct tag_mem_info_t * meminfo = (struct tag_mem_info_t*) tag;
                multiboot_info.meminfo.lower = meminfo->lower * 1024;
                multiboot_info.meminfo.upper = meminfo->upper * 1024;

                break;
                }
            case BIOS_BOOT_DEV_TAG: {
                struct tag_boot_dev_t *dev = (struct tag_boot_dev_t *) tag;
                multiboot_info.bootdev.device = dev->biosdev;
                multiboot_info.bootdev.partition = dev->partition;
                multiboot_info.bootdev.subpartition = dev->subpartition;

                break;
                }
            case MMAP_TAG: {
                buildMMAP((struct tag_mmap_t*) tag);

                break;
                }
            case FRAMEBUFFER_TAG: {
                struct tag_framebuffer_t *frame = (struct tag_framebuffer_t *) tag;
                multiboot_info.framebuf.phys_address = frame->address;
                multiboot_info.framebuf.pitch = frame->pitch;
                multiboot_info.framebuf.width = frame->width;
                multiboot_info.framebuf.height = frame->height;
                multiboot_info.framebuf.bitsPerPixel = frame->bpp;
                multiboot_info.framebuf.colorType = frame->fbtype;
                // set up color info
                switch (multiboot_info.framebuf.colorType) {
                    case 0: {
                        multiboot_info.framebuf.palette.indexedColor.numColors = frame->palette.indexedColor.numColors;
                        for (int i = 0; i < 256; i++) {
                            multiboot_info.framebuf.palette.indexedColor.palette[i] = frame->palette.indexedColor.palette[i];
                        }
                        break;
                    }
                    case 1: {
                        multiboot_info.framebuf.palette.rgbColor = frame->palette.rgbColor;
                        break;
                    }
                    default:
                        break;
                }
                break;
                }
            case ACPI_OLD_RDSP_TAG: {
                // check if we have already processed tag 15
                if (multiboot_info.ACPI_RSDP.Address == 0) {
                    struct tag_RSDP_t *rsdp = (struct tag_RSDP_t *) tag;
                    for (int i = 0; i < (int) sizeof(multiboot_info.ACPI_RSDP.Signature); i++) {
                        multiboot_info.ACPI_RSDP.Signature[i] = rsdp->Signature[i];
                    }
                    multiboot_info.ACPI_RSDP.Checksum = rsdp->Checksum;
                    for (int i = 0; i < (int)sizeof(multiboot_info.ACPI_RSDP.OEMID); i++) {
                        multiboot_info.ACPI_RSDP.OEMID[i] = rsdp->OEMID[i];
                    }  
                    multiboot_info.ACPI_RSDP.Revision = rsdp->Revision;
                    multiboot_info.ACPI_RSDP.Address = rsdp->RsdtAddress;
                }

                break;
            }
            case ACPI_NEW_RDSP_TAG: {
                struct tag_XSDP_t *rsdp = (struct tag_XSDP_t *) tag;
                for (int i = 0; i < (int) sizeof(multiboot_info.ACPI_RSDP.Signature); i++) {
                    multiboot_info.ACPI_RSDP.Signature[i] = rsdp->Signature[i];
                }
                multiboot_info.ACPI_RSDP.Checksum = rsdp->Checksum;
                for (int i = 0; i < (int)sizeof(multiboot_info.ACPI_RSDP.OEMID); i++) {
                    multiboot_info.ACPI_RSDP.OEMID[i] = rsdp->OEMID[i];
                } 
                multiboot_info.ACPI_RSDP.Revision = rsdp->Revision;
                multiboot_info.ACPI_RSDP.Address = rsdp->XsdtAddress;
                multiboot_info.ACPI_RSDP.Length = rsdp->Length;
                multiboot_info.ACPI_RSDP.ExtendedChecksum = rsdp->ExtendedChecksum;
    
               break;
            }
            
            case IMAGE_LOAD_BASE_TAG: {
                struct tag_image_base_phy_addr_t *img = (struct tag_image_base_phy_addr_t *) tag;
                multiboot_info.basephyaddr = img->basephyaddr;
                break;
            }
            case POWER_MGMT_TAG: {
                struct tag_APM_t * apm = (struct tag_APM_t *) tag;
                multiboot_info.APM.version = apm->version;
                multiboot_info.APM.cseg = apm->cseg;
                multiboot_info.APM.dseg = apm->dseg;
                multiboot_info.APM.flags = apm->flags;
                multiboot_info.APM.cseg_len = apm->cseg_len;
                multiboot_info.APM.dseg_len = apm->dseg_len;
                break;
            }
            // purposely ignored
            case 9:
                break;
            default:
                printf("Multiboot tag: %ul skipped\n\r",tag->type);
                break;
        }

        tag = (struct tag_t *) ((uint8_t *)tag + align8(tag->size));
    }

    return true;
}