#include "network.h"
#include "../drivers/screen.h"
#include "../fs/vfs.h"
#include "../lib/string.h"
#include "../honeypot/honeypot.h"

typedef struct {
    unsigned int packets_seen;
    unsigned int alerts_raised;
    unsigned int seed;
} network_state;

static network_state state;

static unsigned int prng_next(void) {
    state.seed = (state.seed * 1664525u + 1013904223u);
    return state.seed;
}

void network_init(void) {
    state.packets_seen = 0;
    state.alerts_raised = 0;
    state.seed = 0xBEEF1234;
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
    print_string("\n");
}

