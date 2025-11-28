#include "memory.h"

void *memset(void *dest, int value, uint32_t len) {
    unsigned char *ptr = (unsigned char *)dest;
    for (uint32_t i = 0; i < len; i++) {
        ptr[i] = (unsigned char)value;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, uint32_t len) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (uint32_t i = 0; i < len; i++) {
        d[i] = s[i];
    }
    return dest;
}

void *memmove(void *dest, const void *src, uint32_t len) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    if (d == s) {
        return dest;
    }

    if (d < s) {
        for (uint32_t i = 0; i < len; i++) {
            d[i] = s[i];
        }
    } else {
        for (uint32_t i = len; i != 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dest;
}

