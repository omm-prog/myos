; Multiboot2 header definition (placed in .multiboot_header section)
section .multiboot_header
align 8

%define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
%define MULTIBOOT2_ARCHITECTURE 0

multiboot_header_start:
    dd MULTIBOOT2_HEADER_MAGIC          ; magic
    dd MULTIBOOT2_ARCHITECTURE          ; architecture (i386)
    dd multiboot_header_end - multiboot_header_start
    dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE + (multiboot_header_end - multiboot_header_start))

    ; mandatory end tag (type = 0)
    align 8
    dw 0
    dw 0
    dd 8

multiboot_header_end:

