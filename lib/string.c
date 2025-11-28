#include "string.h"

static void reverse(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char *)a - *(const unsigned char *)b;
}

int strncmp(const char *a, const char *b, int n) {
    int i = 0;
    while (i < n && a[i] && b[i]) {
        if (a[i] != b[i]) {
            return ((unsigned char)a[i]) - ((unsigned char)b[i]);
        }
        i++;
    }
    if (i == n) {
        return 0;
    }
    return ((unsigned char)a[i]) - ((unsigned char)b[i]);
}

char *strcpy(char *dest, const char *src) {
    char *result = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return result;
}

char *strncpy(char *dest, const char *src, int n) {
    int i = 0;
    for (; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }
    if (c == '\0') {
        return (char *)s;
    }
    return 0;
}

char *strrchr(const char *s, int c) {
    const char *last = 0;
    while (*s) {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    if (c == '\0') {
        return (char *)s;
    }
    return (char *)last;
}

char *strcat(char *dest, const char *src) {
    char *result = dest;
    while (*dest) {
        dest++;
    }
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return result;
}

void int_to_ascii(int value, char *buffer) {
    int i = 0;
    int is_negative = 0;

    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    while (value != 0) {
        int remainder = value % 10;
        buffer[i++] = (char)('0' + remainder);
        value /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';
    reverse(buffer, i);
}

void uint_to_ascii(uint32_t value, char *buffer) {
    int i = 0;
    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }
    while (value != 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    buffer[i] = '\0';
    reverse(buffer, i);
}

void hex_to_ascii(uint32_t value, char *buffer) {
    const char *digits = "0123456789ABCDEF";
    int i = 0;
    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }
    while (value != 0) {
        buffer[i++] = digits[value & 0xF];
        value >>= 4;
    }
    buffer[i] = '\0';
    reverse(buffer, i);
}

