#ifndef HONEYPOT_H
#define HONEYPOT_H

#define HONEYPOT_PORT_SSH 22
#define HONEYPOT_PORT_HTTP 80
#define HONEYPOT_PORT_FTP 21

void honeypot_init();
void honeypot_status();
void honeypot_monitor_ports();
void log_attack(char* source, int port, char* attack_type);

#endif