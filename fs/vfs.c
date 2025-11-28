#include "vfs.h"
#include "../drivers/screen.h"
#include "../lib/string.h"

typedef struct {
    char name[32];
    char contents[256];
} vfs_entry;

static vfs_entry vfs_table[] = {
    {"readme.txt", "MyOS virtual filesystem\nType 'cat readme.txt'\n"},
    {"honeypot.log", "No attacks recorded.\n"},
    {"auto.log", "Auto-system online.\n"}
};

static int vfs_entries = sizeof(vfs_table) / sizeof(vfs_entry);

void vfs_init(void) {
    print_string("Initializing virtual filesystem...\n");
}

void vfs_list(void) {
    print_string("Files:\n");
    for (int i = 0; i < vfs_entries; i++) {
        print_string("  - ");
        print_string(vfs_table[i].name);
        print_string("\n");
    }
}

const char *vfs_read(const char *name) {
    for (int i = 0; i < vfs_entries; i++) {
        if (strcmp(name, vfs_table[i].name) == 0) {
            return vfs_table[i].contents;
        }
    }
    return 0;
}

void vfs_append(const char *name, const char *line) {
    for (int i = 0; i < vfs_entries; i++) {
        if (strcmp(name, vfs_table[i].name) == 0) {
            if (strcmp(vfs_table[i].contents, "No attacks recorded.\n") == 0 &&
                strcmp(name, "honeypot.log") == 0) {
                vfs_table[i].contents[0] = '\0';
            }
            int current_len = strlen(vfs_table[i].contents);
            int line_len = strlen(line);
            if (current_len + line_len + 2 >= (int)sizeof(vfs_table[i].contents)) {
                return;
            }
            strcat(vfs_table[i].contents, line);
            strcat(vfs_table[i].contents, "\n");
            return;
        }
    }
}

