// my kernel header

#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdint.h>
#include <stddef.h>

#define KERNEL_VERSION      "0.000"

extern const char*              BOOTNAME;
extern const char*              BOOTVERSION;
extern const char*              BOOTDATE;
extern uint32_t                 BOOTSTATUS;             // status of boot tasks

#endif