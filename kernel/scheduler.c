#include "scheduler.h"
#include "../drivers/screen.h"
#include "../lib/string.h"

typedef struct {
    int in_use;
    char name[24];
    task_callback callback;
    int interval;
    int counter;
} task_entry;

static task_entry tasks[MAX_TASKS];
static unsigned int ticks = 0;

void scheduler_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].in_use = 0;
    }
    ticks = 0;
}

int scheduler_add_task(const char *name, task_callback callback, int interval_ticks) {
    if (!callback || interval_ticks <= 0) {
        return -1;
    }
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tasks[i].in_use) {
            tasks[i].in_use = 1;
            strncpy(tasks[i].name, name, sizeof(tasks[i].name) - 1);
            tasks[i].name[sizeof(tasks[i].name) - 1] = '\0';
            tasks[i].callback = callback;
            tasks[i].interval = interval_ticks;
            tasks[i].counter = interval_ticks;
            return 0;
        }
    }
    return -1;
}

void scheduler_tick(void) {
    ticks++;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!tasks[i].in_use) {
            continue;
        }
        tasks[i].counter--;
        if (tasks[i].counter <= 0) {
            tasks[i].callback();
            tasks[i].counter = tasks[i].interval;
        }
    }
}

void scheduler_list(void) {
    print_string("Scheduled tasks:\n");
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].in_use) {
            print_string("  - ");
            print_string(tasks[i].name);
            print_string(" (interval ");
            print_int(tasks[i].interval);
            print_string(" ticks, next in ");
            print_int(tasks[i].counter);
            print_string(")\n");
        }
    }
    if (ticks == 0) {
        print_string("No ticks processed yet.\n");
    }
}

