#include "io.h"

uint8_t port_byte_in(uint16_t port) {
    uint8_t result;
    asm volatile("inb %%dx, %%al" : "=a"(result) : "d"(port));
    return result;
}

void port_byte_out(uint16_t port, uint8_t data) {
    asm volatile("outb %%al, %%dx" : : "a"(data), "d"(port));
}

void io_wait(void) {
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

