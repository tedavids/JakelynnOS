// my kernel header

#ifndef _KERNEL_H
#define _KERNEL_H

#include <stdint.h>
#include <stddef.h>

#define KERNEL_VERSION      "0.000"

extern const char*              OSNAME;
extern const char*              OSVERSION;
extern const char*              OSDATE;
extern uint32_t                 BOOTSTATUS;             // status of boot tasks

#endif