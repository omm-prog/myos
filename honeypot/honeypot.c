#include "honeypot.h"
#include "../drivers/screen.h"
#include "../drivers/serial.h"
#include "../fs/vfs.h"
#include "../lib/string.h"

typedef struct {
    int port;
    char *service_name;
    int attack_count;
} honeypot_service;

static honeypot_service services[] = {
    {HONEYPOT_PORT_SSH, "SSH", 0},
    {HONEYPOT_PORT_HTTP, "HTTP", 0},
    {HONEYPOT_PORT_FTP, "FTP", 0}
};

static int total_attacks = 0;

void honeypot_init() {
    print_string("  - Port 22 (SSH) honeypot\n");
    print_string("  - Port 80 (HTTP) honeypot\n");
    print_string("  - Port 21 (FTP) honeypot\n");
    print_string("Honeypot system: ACTIVE\n");
}

void honeypot_status() {
    print_string("\n=== HONEYPOT STATUS ===\n");
    print_string("Total attacks detected: ");
    print_int(total_attacks);
    print_string("\nMonitored services:\n");
    for (int i = 0; i < 3; i++) {
        print_string("  ");
        print_string(services[i].service_name);
        print_string(" (Port ");
        print_int(services[i].port);
        print_string("): ");
        print_int(services[i].attack_count);
        print_string(" attacks\n");
    }
    print_string("======================\n");
}

void honeypot_monitor_ports() {
    /* placeholder for future network integration */
}

void log_attack(char *source, int port, char *attack_type) {
    total_attacks++;

    for (int i = 0; i < 3; i++) {
        if (services[i].port == port) {
            services[i].attack_count++;
            break;
        }
    }

    char log_line[128];
    char port_buf[16];
    uint_to_ascii((uint32_t)port, port_buf);

    strcpy(log_line, "[ALERT] ");
    strcat(log_line, attack_type);
    strcat(log_line, " from ");
    strcat(log_line, source);
    strcat(log_line, " on port ");
    strcat(log_line, port_buf);

    vfs_append("honeypot.log", log_line);
    serial_write_string(log_line);
    serial_write_char('\n');

    print_string("\n[SECURITY ALERT] ");
    print_string(log_line);
    print_string("\n");
}
