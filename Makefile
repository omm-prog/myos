# Compiler and flags
CC = gcc
ASM = nasm

CFLAGS = -m32 -ffreestanding -fno-builtin -fno-stack-protector \
         -nostdlib -nostdinc -Wall -Wextra -std=gnu99 -fno-pie -fno-pic -c
ASMFLAGS = -f elf
LDFLAGS = -m32 -nostdlib -nostdinc -ffreestanding -T linker.ld -no-pie

KERNEL_SOURCES = \
	kernel/kernel.c \
	kernel/terminal.c \
	drivers/screen.c \
	drivers/keyboard.c \
	drivers/serial.c \
	drivers/io.c \
	honeypot/honeypot.c \
	auto/auto.c \
	fs/vfs.c \
	lib/string.c \
	lib/memory.c

ASSEMBLY_SOURCES = boot/boot.asm

KERNEL_OBJECTS = $(KERNEL_SOURCES:.c=.o)
ASSEMBLY_OBJECTS = $(ASSEMBLY_SOURCES:.asm=.o)

KERNEL = myos.bin
ISO = myos.iso

all: $(KERNEL)

$(KERNEL): $(ASSEMBLY_OBJECTS) $(KERNEL_OBJECTS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(ASSEMBLY_OBJECTS) $(KERNEL_OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

iso: $(KERNEL) grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o $(ISO) isodir

run: iso
	qemu-system-x86_64 -cdrom $(ISO)

clean:
	rm -f $(KERNEL) $(ISO) $(KERNEL_OBJECTS) $(ASSEMBLY_OBJECTS)
	rm -rf isodir

.PHONY: all iso run clean
