// header for test routines for kernel
#ifndef _TESTS_H
#define _TESTS_H

// enable tests
#define INCLUDE_TESTS

#define MULTIBOOT_TESTS
#undef PMM_TESTS
#undef VMM_TESTS


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

#endif