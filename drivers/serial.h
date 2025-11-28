#ifndef SERIAL_H
#define SERIAL_H

#include "../lib/types.h"

void serial_init(void);
void serial_write_char(char c);
void serial_write_string(const char *str);
void serial_write_hex(uint32_t value);

#endif

