## JakelynnOS
This is my second play project.  The first one JakelynnOS had too many bad design decisions.

This OS will be x86 32 bit based, I may try a x86_64 one later on

I'm hoping to correct many of these in this project.  Major improvements in design will include:

* Create my memory map FIRST - Done
* Create my page directory and page table entries up front - Done
* Move the kernel to the higher half immediately - Done
* Create my GDT in the .data section so it is preloaded - Done

Full disclosure:  While I was a professional developer on the IBMi(AS/400) I have never been formally trained in C\C++ or x86 assembler

## Features

* Uses GRUB2 and multiboot 2 standard
* Written in GNU assembly and C.  I will use the C23 standard
* I have all warnings set on and have warnings set as errors, I do this to ensure I don't make assumptsion
* In general I will try to have my includes mirror Linux, to make it easier for others to understand

## TODOs

* Get test scripts written
  * libc - not started
  * libkrnl
    * Multiboot tests
  * libintr - not started
  * libcmd
* Get Virtual Memory Manager working -- Complete
* Get backspace working on keyboard input routine, plus move to kbd.c -- Complete
* Get all the frame buffer info from MULTIBOOT2
* Print all multiboot info
* Set up Heap



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

## License 
Distributed under GPL3 