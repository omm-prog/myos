#include "network.h"
#include "../drivers/screen.h"
#include "../fs/vfs.h"
#include "../lib/string.h"
#include "../honeypot/honeypot.h"

typedef struct {
    unsigned int packets_seen;
    unsigned int alerts_raised;
    unsigned int seed;
    char ip_address[16];
} network_state;

static network_state state;
static const char *IP_CONFIG_PATH = "/network/ip.cfg";

static unsigned int prng_next(void) {
    state.seed = (state.seed * 1664525u + 1013904223u);
    return state.seed;
}

void network_init(void) {
    state.packets_seen = 0;
    state.alerts_raised = 0;
    state.seed = 0xBEEF1234;
    vfs_mkdir("/network");
    const char *saved = vfs_read(IP_CONFIG_PATH);
    if (saved && strlen(saved) < sizeof(state.ip_address)) {
        strcpy(state.ip_address, saved);
    } else {
        strcpy(state.ip_address, "0.0.0.0");
        vfs_create(IP_CONFIG_PATH, state.ip_address);
    }
    vfs_append("/security.log", "Network monitor initialized.");
}

void network_poll(void) {
    unsigned int value = prng_next();
    state.packets_seen++;

    if ((value & 0xFF) < 16) {
        int port = 20 + (value % 80);
        const char *type = (value & 0x100) ? "scan" : "brute-force";
        network_simulate_event("remote", port, type);
    }
}

void network_simulate_event(const char *source, int port, const char *type) {
    state.alerts_raised++;
    char log_line[128];
    strcpy(log_line, "Network event ");
    strcat(log_line, type);
    strcat(log_line, " from ");
    strcat(log_line, source);
    strcat(log_line, " on port ");
    char port_buf[16];
    uint_to_ascii((uint32_t)port, port_buf);
    strcat(log_line, port_buf);

    vfs_append("/security.log", log_line);
    log_attack((char *)source, port, (char *)type);
}

void network_status(void) {
    print_string("Network monitor:\n");
    print_string("  Packets seen: ");
    print_int(state.packets_seen);
    print_string("\n  Alerts raised: ");
    print_int(state.alerts_raised);
    print_string("\n  Local IP: ");
    print_string(state.ip_address);
    print_string("\n");
}

const char *network_get_ip(void) {
    return state.ip_address;
}

static int validate_ip(const char *ip) {
    int segments = 0;
    int value = 0;
    int digits = 0;

    while (*ip) {
        if (*ip >= '0' && *ip <= '9') {
            value = value * 10 + (*ip - '0');
            if (value > 255) {
                return 0;
            }
            digits++;
        } else if (*ip == '.') {
            if (digits == 0) {
                return 0;
            }
            segments++;
            value = 0;
            digits = 0;
        } else {
            return 0;
        }
        ip++;
    }
    return (segments == 3 && digits > 0);
}

int network_set_ip(const char *ip) {
    if (!ip || !validate_ip(ip)) {
        return -1;
    }
    strncpy(state.ip_address, ip, sizeof(state.ip_address) - 1);
    state.ip_address[sizeof(state.ip_address) - 1] = '\0';
    vfs_create(IP_CONFIG_PATH, state.ip_address);
    vfs_write(IP_CONFIG_PATH, state.ip_address);
    vfs_append("/security.log", "IP address updated.");
    return 0;
}

