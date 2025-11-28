#include "terminal.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include "../honeypot/honeypot.h"
#include "../auto/auto.h"
#include "../fs/vfs.h"
#include "../net/network.h"
#include "scheduler.h"

#define MAX_PATH_SEGMENTS 16

static char current_dir[VFS_PATH_LEN] = "/";

static void terminal_handle_command(char *input);
static void show_help(void);
static void print_prompt(void);
static char *skip_spaces(char *s);
static void trim_right(char *s);
static int resolve_path(const char *base, const char *input, char *output);
static void handle_ls(char *args);
static void handle_cd(char *args);
static void handle_mkdir(char *args);
static void handle_touch(char *args);
static void handle_write(char *args);
static void handle_cat(char *args);
static void handle_scan(char *args);
static void handle_simulate(char *args);
static void handle_alerts(void);
static void handle_threatlog(void);
static void handle_tasks(void);
static void handle_tick(void);
static void handle_netstat(void);
static int parse_int(const char *text);

void terminal_init(void) {
    print_string("\n=================================\n");
    print_string("    MyOS - Minimal Operating System\n");
    print_string("    Honeypot: ACTIVE\n");
    print_string("    Auto-System: READY\n");
    print_string("=================================\n\n");
    print_string("Type 'help' to list commands.\n\n");
}

void terminal_run(void) {
    char input[128];
    while (1) {
        scheduler_tick();
        print_prompt();
        read_string(input, sizeof(input));
        terminal_handle_command(input);
    }
}

static void terminal_handle_command(char *input) {
    char *cmd = skip_spaces(input);
    trim_right(cmd);

    if (*cmd == '\0') {
        return;
    } else if (strcmp(cmd, "help") == 0) {
        show_help();
    } else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    } else if (strncmp(cmd, "ls", 2) == 0 && (cmd[2] == '\0' || cmd[2] == ' ' || cmd[2] == '\t')) {
        handle_ls(cmd + 2);
    } else if (strncmp(cmd, "cd", 2) == 0 && (cmd[2] == '\0' || cmd[2] == ' ' || cmd[2] == '\t')) {
        handle_cd(cmd + 2);
    } else if (strncmp(cmd, "mkdir", 5) == 0 && (cmd[5] == '\0' || cmd[5] == ' ' || cmd[5] == '\t')) {
        handle_mkdir(cmd + 5);
    } else if (strcmp(cmd, "honeypot") == 0) {
        honeypot_status();
    } else if (strcmp(cmd, "auto") == 0) {
        auto_status();
    } else if (strncmp(cmd, "cat", 3) == 0 && (cmd[3] == '\0' || cmd[3] == ' ' || cmd[3] == '\t')) {
        handle_cat(cmd + 3);
    } else if (strncmp(cmd, "touch", 5) == 0 && (cmd[5] == '\0' || cmd[5] == ' ' || cmd[5] == '\t')) {
        handle_touch(cmd + 5);
    } else if (strncmp(cmd, "write", 5) == 0 && (cmd[5] == '\0' || cmd[5] == ' ' || cmd[5] == '\t')) {
        handle_write(cmd + 5);
    } else if (strcmp(cmd, "alerts") == 0) {
        handle_alerts();
    } else if (strcmp(cmd, "threatlog") == 0) {
        handle_threatlog();
    } else if (strcmp(cmd, "tasks") == 0) {
        handle_tasks();
    } else if (strcmp(cmd, "tick") == 0) {
        handle_tick();
    } else if (strcmp(cmd, "netstat") == 0) {
        handle_netstat();
    } else if (strncmp(cmd, "scan", 4) == 0 && (cmd[4] == '\0' || cmd[4] == ' ' || cmd[4] == '\t')) {
        handle_scan(cmd + 4);
    } else if (strncmp(cmd, "simulate", 8) == 0 && (cmd[8] == '\0' || cmd[8] == ' ' || cmd[8] == '\t')) {
        handle_simulate(cmd + 8);
    } else if (strcmp(cmd, "reboot") == 0) {
        print_string("Rebooting...\n");
        asm volatile("int $0x19");
    } else {
        print_string("Unknown command. Type 'help'.\n");
    }
}

static void show_help(void) {
    print_string("Commands:\n");
    print_string("  help           Show this help message\n");
    print_string("  clear          Clear the display\n");
    print_string("  ls [dir]       List directory contents\n");
    print_string("  cd <dir>       Change directory\n");
    print_string("  mkdir <dir>    Create a directory\n");
    print_string("  touch <file>   Create an empty file\n");
    print_string("  write <f> <t>  Replace file contents\n");
    print_string("  cat <file>     View file contents\n");
    print_string("  honeypot       Show honeypot status\n");
    print_string("  alerts         Honeypot status + latest log\n");
    print_string("  threatlog      View security log\n");
    print_string("  scan <target>  Simulate threat scan\n");
    print_string("  simulate <p>   Simulate attack on port\n");
    print_string("  netstat        Show network monitor stats\n");
    print_string("  tasks          List scheduled tasks\n");
    print_string("  tick           Advance scheduler manually\n");
    print_string("  auto           Auto-system status\n");
    print_string("  reboot         Return to firmware\n");
}

static void print_prompt(void) {
    print_string("myos");
    print_string(current_dir);
    print_string("> ");
}

static char *skip_spaces(char *s) {
    while (*s == ' ' || *s == '\t') {
        s++;
    }
    return s;
}

static void trim_right(char *s) {
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[--len] = '\0';
    }
}

static void handle_touch(char *args) {
    char *name = skip_spaces(args);
    trim_right(name);

    if (*name == '\0') {
        print_string("Usage: touch <filename>\n");
        return;
    }

    char path[VFS_PATH_LEN];
    if (resolve_path(current_dir, name, path) != 0) {
        print_string("Path too long.\n");
        return;
    }

    if (vfs_create(path, 0) == 0) {
        print_string("File ready: ");
        print_string(path);
        print_string("\n");
    } else {
        print_string("Failed to create file.\n");
    }
}

static void handle_write(char *args) {
    char *name = skip_spaces(args);
    char *space = strchr(name, ' ');

    if (!space) {
        print_string("Usage: write <file> <text>\n");
        return;
    }

    *space = '\0';
    char *text = skip_spaces(space + 1);

    if (*name == '\0' || *text == '\0') {
        print_string("Usage: write <file> <text>\n");
        return;
    }

    char path[VFS_PATH_LEN];
    if (resolve_path(current_dir, name, path) != 0) {
        print_string("Path too long.\n");
        return;
    }

    if (!vfs_exists(path)) {
        if (vfs_create(path, "") != 0) {
            print_string("Cannot create file.\n");
            return;
        }
    }

    if (vfs_write(path, text) == 0) {
        print_string("Updated file: ");
        print_string(path);
        print_string("\n");
    } else {
        print_string("Failed to write file.\n");
    }
}

static void handle_cat(char *args) {
    char *name = skip_spaces(args);
    trim_right(name);

    if (*name == '\0') {
        print_string("Usage: cat <file>\n");
        return;
    }

    char path[VFS_PATH_LEN];
    if (resolve_path(current_dir, name, path) != 0) {
        print_string("Path too long.\n");
        return;
    }

    const char *data = vfs_read(path);
    if (data) {
        int len = strlen(data);
        print_string(data);
        if (len == 0 || data[len - 1] != '\n') {
            print_string("\n");
        }
    } else {
        print_string("File not found.\n");
    }
}

static void handle_ls(char *args) {
    char *target = skip_spaces(args);
    if (*target == '\0') {
        target = ".";
    }

    char path[VFS_PATH_LEN];
    if (resolve_path(current_dir, target, path) != 0) {
        print_string("Path too long.\n");
        return;
    }

    vfs_list(path);
}

static void handle_cd(char *args) {
    char *target = skip_spaces(args);
    if (*target == '\0') {
        target = "/";
    }

    char path[VFS_PATH_LEN];
    if (resolve_path(current_dir, target, path) != 0) {
        print_string("Path too long.\n");
        return;
    }

    if (!vfs_is_dir(path)) {
        print_string("Directory not found.\n");
        return;
    }

    strncpy(current_dir, path, VFS_PATH_LEN - 1);
    current_dir[VFS_PATH_LEN - 1] = '\0';
}

static void handle_mkdir(char *args) {
    char *target = skip_spaces(args);
    trim_right(target);

    if (*target == '\0') {
        print_string("Usage: mkdir <path>\n");
        return;
    }

    char path[VFS_PATH_LEN];
    if (resolve_path(current_dir, target, path) != 0) {
        print_string("Path too long.\n");
        return;
    }

    if (vfs_mkdir(path) == 0) {
        print_string("Directory ready: ");
        print_string(path);
        print_string("\n");
    } else {
        print_string("Failed to create directory.\n");
    }
}

static void handle_scan(char *args) {
    char *target = skip_spaces(args);
    trim_right(target);

    if (*target == '\0') {
        print_string("Usage: scan <target>\n");
        return;
    }

    print_string("Scanning target ");
    print_string(target);
    print_string(" for open ports...\n");
    print_string("Analysis complete: no critical vulnerabilities detected.\n");

    char log_line[96];
    strcpy(log_line, "Scan report for ");
    strcat(log_line, target);
    strcat(log_line, ": clean");
    vfs_append("/security.log", log_line);
}

static void handle_simulate(char *args) {
    char *port_text = skip_spaces(args);
    trim_right(port_text);

    if (*port_text == '\0') {
        print_string("Usage: simulate <port>\n");
        return;
    }

    int port = parse_int(port_text);
    if (port <= 0) {
        print_string("Invalid port number.\n");
        return;
    }

    print_string("Simulating attack on port ");
    print_int(port);
    print_string("...\n");
    log_attack("simulation", port, "test");
}

static void handle_alerts(void) {
    honeypot_status();
    const char *log = vfs_read("/honeypot.log");
    if (log) {
        print_string("--- honeypot.log ---\n");
        print_string(log);
    }
}

static void handle_threatlog(void) {
    const char *log = vfs_read("/security.log");
    if (log) {
        print_string("--- security.log ---\n");
        print_string(log);
    } else {
        print_string("Security log missing.\n");
    }
}

static void handle_tasks(void) {
    scheduler_list();
}

static void handle_tick(void) {
    scheduler_tick();
    print_string("Tick executed.\n");
}

static void handle_netstat(void) {
    network_status();
}

static int parse_int(const char *text) {
    int value = 0;
    while (*text >= '0' && *text <= '9') {
        value = value * 10 + (*text - '0');
        text++;
    }
    return value;
}

static int resolve_path(const char *base, const char *input, char *output) {
    char buffer[VFS_PATH_LEN];

    if (input[0] == '\0') {
        strncpy(buffer, base, VFS_PATH_LEN - 1);
        buffer[VFS_PATH_LEN - 1] = '\0';
    } else if (input[0] == '/') {
        if ((int)strlen(input) >= VFS_PATH_LEN) {
            return -1;
        }
        strcpy(buffer, input);
    } else {
        int base_len = strlen(base);
        int input_len = strlen(input);
        int extra = (strcmp(base, "/") == 0) ? 0 : 1;
        if (base_len + extra + input_len >= VFS_PATH_LEN) {
            return -1;
        }
        strcpy(buffer, base);
        if (strcmp(base, "/") != 0) {
            buffer[base_len] = '/';
            buffer[base_len + 1] = '\0';
        } else {
            buffer[1] = '\0';
        }
        strcat(buffer, input);
    }

    char segments[MAX_PATH_SEGMENTS][VFS_NAME_LEN];
    int segment_count = 0;
    char *p = buffer;

    while (*p) {
        while (*p == '/') {
            p++;
        }
        if (*p == '\0') {
            break;
        }
        char token[VFS_NAME_LEN];
        int idx = 0;
        while (*p && *p != '/' && idx < VFS_NAME_LEN - 1) {
            token[idx++] = *p++;
        }
        token[idx] = '\0';
        if (idx >= VFS_NAME_LEN - 1 && *p && *p != '/') {
            return -1;
        }
        if (strcmp(token, ".") == 0) {
            continue;
        }
        if (strcmp(token, "..") == 0) {
            if (segment_count > 0) {
                segment_count--;
            }
            continue;
        }
        if (segment_count >= MAX_PATH_SEGMENTS) {
            return -1;
        }
        strcpy(segments[segment_count++], token);
    }

    if (segment_count == 0) {
        output[0] = '/';
        output[1] = '\0';
        return 0;
    }

    output[0] = '\0';
    for (int i = 0; i < segment_count; i++) {
        if ((int)strlen(output) + 1 + (int)strlen(segments[i]) >= VFS_PATH_LEN) {
            return -1;
        }
        strcat(output, "/");
        strcat(output, segments[i]);
    }
    return 0;
}
