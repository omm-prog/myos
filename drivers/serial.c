#include "serial.h"
#include "io.h"
#include "../lib/string.h"

#define SERIAL_COM1_BASE 0x3F8

static int serial_is_transmit_empty(void) {
    return port_byte_in(SERIAL_COM1_BASE + 5) & 0x20;
}

void serial_init(void) {
    port_byte_out(SERIAL_COM1_BASE + 1, 0x00);
    port_byte_out(SERIAL_COM1_BASE + 3, 0x80);
    port_byte_out(SERIAL_COM1_BASE + 0, 0x03);
    port_byte_out(SERIAL_COM1_BASE + 1, 0x00);
    port_byte_out(SERIAL_COM1_BASE + 3, 0x03);
    port_byte_out(SERIAL_COM1_BASE + 2, 0xC7);
    port_byte_out(SERIAL_COM1_BASE + 4, 0x0B);
}

void serial_write_char(char c) {
    while (serial_is_transmit_empty() == 0) {
        /* wait */
    }
    port_byte_out(SERIAL_COM1_BASE, (unsigned char)c);
}

void serial_write_string(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        serial_write_char(str[i]);
    }
}

void serial_write_hex(uint32_t value) {
    char buffer[12];
    hex_to_ascii(value, buffer);
    serial_write_string("0x");
    serial_write_string(buffer);
}

