#include "keyboard.h"
#include "io.h"

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

#define SCAN_CODE_RELEASE    0x80
#define SCAN_CODE_LEFT_SHIFT 0x2A
#define SCAN_CODE_RIGHT_SHIFT 0x36

static int shift_active = 0;

static const char keymap[] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    0
};

static const char keymap_shifted[] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0,'|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
    0
};

static unsigned char keyboard_read_scancode(void) {
    while (!(port_byte_in(KEYBOARD_STATUS_PORT) & 0x01)) {
        /* wait for data */
    }
    return port_byte_in(KEYBOARD_DATA_PORT);
}

void keyboard_init(void) {
    shift_active = 0;
}

char keyboard_read_char(void) {
    while (1) {
        unsigned char scancode = keyboard_read_scancode();

        if (scancode == SCAN_CODE_LEFT_SHIFT || scancode == SCAN_CODE_RIGHT_SHIFT) {
            shift_active = 1;
            continue;
        }

        if (scancode == (SCAN_CODE_LEFT_SHIFT | SCAN_CODE_RELEASE) ||
            scancode == (SCAN_CODE_RIGHT_SHIFT | SCAN_CODE_RELEASE)) {
            shift_active = 0;
            continue;
        }

        if (scancode & SCAN_CODE_RELEASE) {
            continue;
        }

        if (scancode >= sizeof(keymap)) {
            continue;
        }

        char character = shift_active ? keymap_shifted[scancode] : keymap[scancode];
        return character;
    }
}

