#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void *memset(void *dest, int value, uint32_t len);
void *memcpy(void *dest, const void *src, uint32_t len);
void *memmove(void *dest, const void *src, uint32_t len);

#endif

