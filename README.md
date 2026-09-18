## JakelynnOS
This is my second play project.  The first one JakelynnOS had too many bad design decisions.

This OS will be x86 32 bit based, I may try a x86_64 one later on

I'm hoping to correct many of these in this project.  Major improvements in design will include:

* Create my memory map FIRST - Done
* Create my page directory and page table entries up front - Done
* Move the kernel to the higher half immediately - Done
* Create my GDT in the .data section so it is preloaded - Done

Full disclosure:  While I was a professional developer on the IBMi(AS/400) I have never been formally trained in C\C++ or x86 assembler.  That being said I have written many, many C and C++ programs for interfacing many different systems both on the IBMi, AIX, Linux, and PC

## Features

* Uses GRUB2 and multiboot 2 standard
* Written in GNU assembly and C.  I will use the C23 standard
* I have all warnings set on and have warnings set as errors, I do this to ensure I don't make assumptsion
* In general I will try to have my includes mirror Linux, to make it easier for others to understand

## TODOs

* Get test scripts written
  * libc - not started
  * libkrnl
    * Multiboot tests -- not started
  * libintr - not started
  * libcmd - not started
* Redo Physical Memory Manager...Done
* Get Virtual Memory Manager working based on new PMM - in process
* Print all multiboot info -- Done for now, APM, and 
* Set up Heap
* New 'doCmd' commands
  * print vmmap
  * kloadinfo -- print the kernel load info (.text, .data, .rodata, etc), also include boot.S info
* Set up ACPI memory
  * New type in PMM, find which reserved memory block contains this address
  * Identity map in VMM
  * Add tests
  * give own header (acpi.h) and .c (acpi.c) files
  * Fully set up data structures and functions
  * Add new kernel.c line to set it up (after heap)
* Revisit timer setup, need to make a timer that kicks off every x MS (set by command line?)
* Start working on a scheduler and multithreading

## AI Usage Policy

I use AI for the following:
* code reviews
* gernerate code that I can study, then I write my own code.  I do not use AI generated code, it defeats the purpose of this project which is learningS
* Suggestions on how to implement something

I don't use it for
* generating code to include in this project

## 🛠️ Prerequisite

* you will need an i686 cross compiler
* you can go to osdev.org for how to build this (https://wiki.osdev.org/GCC_Cross-Compiler)

## 📦 Building and Running

* clone the repository
* run the makefile in the project directory

## Credits

* OSDev contributors
* Reddit users
  * Octocontrabass, sebihepp, and nullplan for their help getting to the Higher Half
* https://www.tutorialspoint.com/data_structures_algorithms/avl_tree_algorithm.htm for a self balancing binary tree
* ostep.org Operating systems 3 easy pieces, I also bought the book

## License 
Distributed under GPL3 

## Notes
  * Second pass enhancements
    * Change heap to take one page allocations from the bottom of the heap, and multi page enhancements from the top to reduce fragmentation