#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/serial.h"
#include "../honeypot/honeypot.h"
#include "../auto/auto.h"
#include "../fs/vfs.h"
#include "../net/network.h"
#include "scheduler.h"
#include "terminal.h"

void kernel_main(unsigned int multiboot_magic, unsigned int multiboot_info) {
    (void)multiboot_magic;
    (void)multiboot_info;

    clear_screen();
    serial_init();
    keyboard_init();
    vfs_init();
    honeypot_init();
    auto_init();
    network_init();
    scheduler_init();
    scheduler_add_task("auto", auto_tick, 8);
    scheduler_add_task("honeypot", honeypot_monitor_ports, 12);
    scheduler_add_task("network", network_poll, 6);

    terminal_init();
    terminal_run();
}
