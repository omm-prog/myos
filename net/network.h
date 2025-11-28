#ifndef NETWORK_H
#define NETWORK_H

void network_init(void);
void network_poll(void);
void network_status(void);
void network_simulate_event(const char *source, int port, const char *type);

#endif

