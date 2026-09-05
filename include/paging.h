// paging header

#ifndef _PAGING_H
#define _PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <vmm.h>

#define ALIGN_4K(x) (((uint32_t)(x) + ((uint32_t)4095)) & ~((uint32_t)4095))

extern uint32_t _kernel_end;
extern uint32_t _kernel_start;

extern const uint32_t   PAGINGIMPLEMENTED;
extern uint32_t         page_directory[1024];



// public paging functions



// init the paging structures

// Parameters:  none

// Returns:     true if successful, false otherwise
extern bool initPaging();
#endif