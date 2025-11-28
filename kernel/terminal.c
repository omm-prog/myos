#include "terminal.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include "../honeypot/honeypot.h"
#include "../auto/auto.h"
#include "../fs/vfs.h"

static void terminal_handle_command(char *input);

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
        print_string("myos> ");
        read_string(input, sizeof(input));
        terminal_handle_command(input);
    }
}

static void terminal_handle_command(char *input) {
    if (strcmp(input, "help") == 0) {
        print_string("Commands:\n");
        print_string("  help      Show this help message\n");
        print_string("  clear     Clear the display\n");
        print_string("  honeypot  Show honeypot status\n");
        print_string("  auto      Show auto-system status\n");
        print_string("  ls        List virtual files\n");
        print_string("  cat <f>   View virtual file contents\n");
        print_string("  reboot    Jump back to bootloader\n");
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
    } else if (strcmp(input, "honeypot") == 0) {
        honeypot_status();
    } else if (strcmp(input, "auto") == 0) {
        auto_status();
    } else if (strcmp(input, "ls") == 0) {
        vfs_list();
    } else if (strncmp(input, "cat ", 4) == 0) {
        const char *name = input + 4;
        const char *data = vfs_read(name);
        if (data) {
            print_string(data);
        } else {
            print_string("File not found.\n");
        }
    } else if (strcmp(input, "reboot") == 0) {
        print_string("Rebooting...\n");
        asm volatile("int $0x19");
    } else if (strcmp(input, "") == 0) {
        /* ignore */
    } else {
        print_string("Unknown command. Type 'help'.\n");
    }
}

