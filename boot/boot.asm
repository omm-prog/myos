%include "multiboot_header.asm"

section .text
    global _start
    extern kernel_main

[bits 32]
_start:
    cli
    mov esp, stack_top

    ; Multiboot places the magic value in EAX and the info pointer in EBX.
    push ebx
    push eax
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt

section .bss
align 16
stack_bottom:
    resb 8192
stack_top: