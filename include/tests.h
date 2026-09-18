// header for test routines for kernel

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

#ifndef _TESTS_H
#define _TESTS_H

// enable tests
#define INCLUDE_TESTS

#define MULTIBOOT_TESTS
#undef PMM_TESTS
#define VMM_TESTS
#define HEAP_TESTS

#ifdef MULTIBOOT_TESTS
extern bool multiboottest();
#endif 

#ifdef PMM_TESTS
// physical memory manager tests
extern bool pmmtests();
#endif 

#ifdef VMM_TESTS
// virtual memory manager tests
extern bool vmmtests();
#endif

#ifdef HEAP_TESTS 
// heap tests
extern bool heaptests();
#endif 

#endif