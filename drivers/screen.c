#include "screen.h"
#include "keyboard.h"
#include "io.h"
#include "../lib/string.h"
#include "../lib/memory.h"

static int get_offset(int col, int row);
static int handle_scrolling(int cursor_offset);
static void backspace(void);

void clear_screen(void) {
    char *video_memory = (char *)VIDEO_ADDRESS;
    for (int i = 0; i < MAX_COLS * MAX_ROWS * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(0);
}

void print_char(char character) {
    int offset = get_cursor_offset();

    if (character == '\n') {
        int rows = offset / (2 * MAX_COLS);
        offset = get_offset(0, rows + 1);
    } else if (character == '\b') {
        backspace();
        return;
    } else {
        char *video_memory = (char *)VIDEO_ADDRESS;
        video_memory[offset] = character;
        video_memory[offset + 1] = WHITE_ON_BLACK;
        offset += 2;
    }

    offset = handle_scrolling(offset);
    set_cursor_offset(offset);
}

void print_string(const char *string) {
    for (int i = 0; string[i] != '\0'; i++) {
        print_char(string[i]);
    }
}

void print_int(int value) {
    char buffer[16];
    int_to_ascii(value, buffer);
    print_string(buffer);
}

void print_hex(uint32_t value) {
    char buffer[16];
    hex_to_ascii(value, buffer);
    print_string("0x");
    print_string(buffer);
}

void read_string(char *buffer, int max_length) {
    if (max_length <= 1) {
        buffer[0] = '\0';
        return;
    }
    int i = 0;
    while (1) {
        char c = keyboard_read_char();

        if (c == '\n') {
            print_char('\n');
            buffer[i] = '\0';
            return;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
        } else if (i < max_length - 1) {
            buffer[i++] = c;
            print_char(c);
        }
    }
}

int get_cursor_offset(void) {
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8;
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset |= port_byte_in(REG_SCREEN_DATA);
    return offset * 2;
}

void set_cursor_offset(int offset) {
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xFF));
}

static int get_offset(int col, int row) {
    return 2 * (row * MAX_COLS + col);
}

static int handle_scrolling(int cursor_offset) {
    if (cursor_offset < MAX_ROWS * MAX_COLS * 2) {
        return cursor_offset;
    }

    for (int row = 1; row < MAX_ROWS; row++) {
        char *dest = (char *)VIDEO_ADDRESS + get_offset(0, row - 1);
        char *src = (char *)VIDEO_ADDRESS + get_offset(0, row);
        memmove(dest, src, MAX_COLS * 2);
    }

    char *last_line = (char *)VIDEO_ADDRESS + get_offset(0, MAX_ROWS - 1);
    for (int col = 0; col < MAX_COLS; col++) {
        last_line[col * 2] = ' ';
        last_line[col * 2 + 1] = WHITE_ON_BLACK;
    }

    return get_offset(0, MAX_ROWS - 1);
}

static void backspace(void) {
    int offset = get_cursor_offset();
    if (offset == 0) {
        return;
    }
    offset -= 2;
    char *video_memory = (char *)VIDEO_ADDRESS;
    video_memory[offset] = ' ';
    video_memory[offset + 1] = WHITE_ON_BLACK;
    set_cursor_offset(offset);
}
