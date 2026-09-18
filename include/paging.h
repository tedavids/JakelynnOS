// paging header

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

#ifndef _PAGING_H
#define _PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <vmm.h>

#define ALIGN_4K(x) (((uint32_t)(x) + ((uint32_t)4095)) & ~((uint32_t)4095))

extern uint32_t _kernel_end;
extern uint32_t _kernel_start;

extern const uint32_t   PAGINGIMPLEMENTED;



// public paging functions



// init the paging structures

// Parameters:  none

// Returns:     true if successful, false otherwise
extern bool initPaging();
#endif