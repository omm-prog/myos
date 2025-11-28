#ifndef STRING_H
#define STRING_H

#include "types.h"

int strlen(const char *str);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, int n);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
void int_to_ascii(int value, char *buffer);
void uint_to_ascii(uint32_t value, char *buffer);
void hex_to_ascii(uint32_t value, char *buffer);

#endif

