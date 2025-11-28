#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/serial.h"
#include "../honeypot/honeypot.h"
#include "../auto/auto.h"
#include "../fs/vfs.h"
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

    terminal_init();
    terminal_run();
}
