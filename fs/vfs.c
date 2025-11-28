#include "vfs.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include "../lib/memory.h"

#define VFS_MAX_ENTRIES 32
#define VFS_CONTENT_LEN 512

typedef struct {
    int in_use;
    int is_dir;
    char path[VFS_PATH_LEN];
    char contents[VFS_CONTENT_LEN];
} vfs_entry;

static vfs_entry entries[VFS_MAX_ENTRIES];

static void normalize_path(char *path);
static void make_absolute(const char *input, char *output);
static int find_entry(const char *path);
static int add_entry(const char *path, int is_dir);
static void get_parent(const char *path, char *parent);
static int ensure_parent_exists(const char *path);
static int is_direct_child(const char *parent, const char *child, char *name_out);

void vfs_init(void) {
    memset(entries, 0, sizeof(entries));
    entries[0].in_use = 1;
    entries[0].is_dir = 1;
    strcpy(entries[0].path, "/");

    vfs_create("/readme.txt", "MyOS virtual filesystem\nType 'help' for commands\n");
    vfs_create("/honeypot.log", "No attacks recorded.\n");
    vfs_create("/auto.log", "Auto-system online.\n");
    vfs_create("/security.log", "Security log ready.\n");

    print_string("Initializing virtual filesystem...\n");
}

int vfs_exists(const char *path) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);
    return find_entry(absolute) >= 0;
}

int vfs_is_dir(const char *path) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);
    int idx = find_entry(absolute);
    if (idx < 0) {
        return 0;
    }
    return entries[idx].is_dir;
}

int vfs_mkdir(const char *path) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);
    if (strcmp(absolute, "/") == 0) {
        return 0;
    }

    int idx = find_entry(absolute);
    if (idx >= 0) {
        if (entries[idx].is_dir) {
            return 0;
        }
        return -1;
    }

    if (ensure_parent_exists(absolute) != 0) {
        return -1;
    }

    return add_entry(absolute, 1);
}

int vfs_create(const char *path, const char *initial) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);

    int idx = find_entry(absolute);
    if (idx >= 0) {
        if (entries[idx].is_dir) {
            return -1;
        }
        if (initial) {
            vfs_write(absolute, initial);
        }
        return 0;
    }

    if (ensure_parent_exists(absolute) != 0) {
        return -1;
    }

    if (add_entry(absolute, 0) != 0) {
        return -1;
    }

    if (initial) {
        vfs_write(absolute, initial);
    } else {
        vfs_write(absolute, "");
    }
    return 0;
}

int vfs_write(const char *path, const char *text) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);

    int idx = find_entry(absolute);
    if (idx < 0 || entries[idx].is_dir) {
        return -1;
    }

    int len = strlen(text);
    if (len >= VFS_CONTENT_LEN) {
        len = VFS_CONTENT_LEN - 1;
    }
    for (int i = 0; i < len; i++) {
        entries[idx].contents[i] = text[i];
    }
    entries[idx].contents[len] = '\0';
    return 0;
}

const char *vfs_read(const char *path) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);

    int idx = find_entry(absolute);
    if (idx < 0 || entries[idx].is_dir) {
        return 0;
    }
    return entries[idx].contents;
}

int vfs_list(const char *path) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);

    int idx = find_entry(absolute);
    if (idx < 0 || !entries[idx].is_dir) {
        print_string("Directory not found.\n");
        return -1;
    }

    char child_name[VFS_NAME_LEN];
    int printed = 0;

    for (int i = 0; i < VFS_MAX_ENTRIES; i++) {
        if (!entries[i].in_use) {
            continue;
        }
        if (is_direct_child(absolute, entries[i].path, child_name)) {
            print_string(entries[i].is_dir ? "[DIR] " : "      ");
            print_string(child_name);
            print_string("\n");
            printed = 1;
        }
    }

    if (!printed) {
        print_string("(empty)\n");
    }
    return 0;
}

void vfs_append(const char *path, const char *line) {
    char absolute[VFS_PATH_LEN];
    make_absolute(path, absolute);

    if (!vfs_exists(absolute)) {
        if (vfs_create(absolute, "") != 0) {
            return;
        }
    }

    int idx = find_entry(absolute);
    if (idx < 0 || entries[idx].is_dir) {
        return;
    }

    int current_len = strlen(entries[idx].contents);
    int line_len = strlen(line);
    if (current_len + line_len + 2 >= VFS_CONTENT_LEN) {
        return;
    }

    strcat(entries[idx].contents, line);
    strcat(entries[idx].contents, "\n");
}

static void normalize_path(char *path) {
    int len = strlen(path);
    if (len > 1 && path[len - 1] == '/') {
        path[len - 1] = '\0';
    }
}

static void make_absolute(const char *input, char *output) {
    if (input[0] == '/') {
        strncpy(output, input, VFS_PATH_LEN - 1);
        output[VFS_PATH_LEN - 1] = '\0';
    } else {
        output[0] = '/';
        strncpy(output + 1, input, VFS_PATH_LEN - 2);
        output[VFS_PATH_LEN - 1] = '\0';
    }
    if (output[0] == '\0') {
        output[0] = '/';
        output[1] = '\0';
    }
    normalize_path(output);
    if (output[0] == '\0') {
        output[0] = '/';
        output[1] = '\0';
    }
}

static int find_entry(const char *path) {
    for (int i = 0; i < VFS_MAX_ENTRIES; i++) {
        if (entries[i].in_use && strcmp(entries[i].path, path) == 0) {
            return i;
        }
    }
    return -1;
}

static int add_entry(const char *path, int is_dir) {
    for (int i = 0; i < VFS_MAX_ENTRIES; i++) {
        if (!entries[i].in_use) {
            entries[i].in_use = 1;
            entries[i].is_dir = is_dir;
            strncpy(entries[i].path, path, VFS_PATH_LEN - 1);
            entries[i].path[VFS_PATH_LEN - 1] = '\0';
            entries[i].contents[0] = '\0';
            return 0;
        }
    }
    return -1;
}

static void get_parent(const char *path, char *parent) {
    if (strcmp(path, "/") == 0) {
        strcpy(parent, "/");
        return;
    }

    char temp[VFS_PATH_LEN];
    strncpy(temp, path, VFS_PATH_LEN - 1);
    temp[VFS_PATH_LEN - 1] = '\0';

    char *last_slash = strrchr(temp, '/');
    if (last_slash == temp) {
        parent[0] = '/';
        parent[1] = '\0';
    } else if (last_slash) {
        *last_slash = '\0';
        strncpy(parent, temp, VFS_PATH_LEN - 1);
        parent[VFS_PATH_LEN - 1] = '\0';
    } else {
        parent[0] = '/';
        parent[1] = '\0';
    }
}

static int ensure_parent_exists(const char *path) {
    char parent[VFS_PATH_LEN];
    get_parent(path, parent);
    int idx = find_entry(parent);
    if (idx < 0) {
        return -1;
    }
    if (!entries[idx].is_dir) {
        return -1;
    }
    return 0;
}

static int is_direct_child(const char *parent, const char *child, char *name_out) {
    if (strcmp(parent, child) == 0) {
        return 0;
    }

    const char *relative = 0;

    if (strcmp(parent, "/") == 0) {
        if (child[0] != '/' || child[1] == '\0') {
            return 0;
        }
        relative = child + 1;
    } else {
        int parent_len = strlen(parent);
        if (strncmp(child, parent, parent_len) != 0) {
            return 0;
        }
        if (child[parent_len] != '/') {
            return 0;
        }
        relative = child + parent_len + 1;
    }

    if (!relative || *relative == '\0') {
        return 0;
    }

    if (strchr(relative, '/')) {
        return 0;
    }

    strncpy(name_out, relative, VFS_NAME_LEN - 1);
    name_out[VFS_NAME_LEN - 1] = '\0';
    return 1;
}

