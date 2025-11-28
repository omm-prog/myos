#ifndef IO_H
#define IO_H

#include "../lib/types.h"

uint8_t port_byte_in(uint16_t port);
void port_byte_out(uint16_t port, uint8_t data);
void io_wait(void);

#endif

