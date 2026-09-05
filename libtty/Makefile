# this makefile creates the libtty.a object
DEFAULT_HOST!=../default_host.sh
HOST?=$(DEFAULT_HOST)
HOSTARCH!=../target-triplet-to-arch.sh $(HOST)

TARGET=libtty.a
TARGET2=ttytest
SRCDIR=source
OBJDIR=object
DESTDIR=.
ARCHDIR=../arch/$(HOSTARCH)/
LIBDIR=../lib
ISODIR=iso

ISO = $(DESTDIR)/ttytest.iso

#tool chain
CC=i686-elf-gcc
CPP=i686-elf-g++
AS=i686-elf-gcc
LD=i686-elf-gcc

#initail flags
#CFLAGS ?= -O2 -g 
CFLAGS ?= -std=c23 -g
CPPFLAGS ?=
LDFLAGS ?= 
LIBS ?= -L../lib

#add my flags
CFLAGS := $(CFLAGS) -ffreestanding -Wall -Wextra -pedantic -Wconversion -Wcast-align -Wcast-qual \
		 -Wredundant-decls -Wsign-conversion -Wswitch-default -Wundef -Wfloat-equal -Werror -fPIC
CPPFLAGS := $(CPPFLAGS) -D__is_libtty  -isystem include 
LDFLAGS := $(LDFLAGS) -nostdlib  -Wl,-Map,output.map
LIBS := $(LIBS) -lc -ltty -lgcc -lkrnl -lintr -lkbd -lcmd

ARCHDIR=arch/$(HOSTARCH)

include ../$(ARCHDIR)/make.config

#add architecture specific flags
CFLAGS := $(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS := $(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LIBTTY_CFLAGS := $(LIBTTY_CFLAGS) $(ARCH_CFLAGS)
LIBTTY_CPPFLAGS := $(LIBTTY_CPPFLAGS) $(ARCH_CPPFLAGS)

FREEOBJS=$(OBJDIR)/80x25cursor.o \
$(OBJDIR)/vga.o \
$(OBJDIR)/tty.o \
$(OBJDIR)/ttytests.o \
$(OBJDIR)/vgatests.o \
$(OBJDIR)/cursortests.o \

HOSTEDOBJS = \

OBJS=$(FREEOBJS) \
$(HOSTEDOBJS) \

KERNEL_OBJS = \
$(KERNEL_ARCH_OBJS) \
$(OBJDIR)/test.o \

LINK_LIST = \
$(LDFLAGS) \
$(FREEOBJS) \
../$(KERNEL_OBJS) \
$(LIBS) \




#$(OBJDIR)/crti.o \
../$(OBJDIR)/crtbegin.o \
../$(OBJDIR)/crtn.o \
../$(OBJDIR)/crtend.o \
.PHONY: all clean testtty

all: $(LIBDIR)/$(TARGET) $(ISO)

$(ISO) :  $(DESTDIR)/$(TARGET2) 
	#cp $(TARGET2) $(ISODIR)/boot
	#grub2-mkrescue -o $(ISO) $(ISODIR) 

$(LIBDIR)/$(TARGET) : $(DESTDIR)/$(TARGET)
	cp $(DESTDIR)/$(TARGET) $(LIBDIR)/$(TARGET)

$(DESTDIR)/$(TARGET) : $(OBJS)
	$(AR) rcs $@ $(OBJS)

$(DESTDIR)/$(TARGET2) : $(OBJS) $(OBJDIR)/test.o
#	$(LD) -T ../$(ARCHDIR)/linker.ld -o $(DESTDIR)/$(TARGET2) $(CFLAGS) -Wl,--start-group $(LINK_LIST) -Wl,--end-group 
#	grub2-file --is-x86-multiboot2 ./ttytest

$(OBJDIR)/tty.o : $(SRCDIR)/tty.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(OBJDIR)/vga.o : $(SRCDIR)/vga.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(OBJDIR)/80x25cursor.o : $(SRCDIR)/80x25cursor.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(OBJDIR)/test.o : $(SRCDIR)/test.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(OBJDIR)/cursortests.o : $(SRCDIR)/cursortests.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(OBJDIR)/vgatests.o : $(SRCDIR)/vgatests.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

$(OBJDIR)/ttytests.o : $(SRCDIR)/ttytests.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f $(DESTDIR)/*.a
	rm -f $(OBJDIR)/*.d 
	find ./object -maxdepth 1 -name "*.o" -type f -delete
	rm -f ttytest
	rm -f ttytest.iso

testtty:
	qemu-system-i386 -readconfig qemu.conf -no-reboot -no-shutdown

debug:
	#qemu-system-i386 ./ttytest -readconfig qemu.conf -no-reboot -no-shutdown -s -S
