#ifndef SCREEN_H
#define SCREEN_H

#include "../lib/types.h"

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

void clear_screen(void);
void print_string(const char *string);
void print_char(char character);
void print_int(int value);
void print_hex(uint32_t value);
void read_string(char *buffer, int max_length);
int get_cursor_offset(void);
void set_cursor_offset(int offset);

#endif
