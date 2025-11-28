#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_TASKS 8

typedef void (*task_callback)(void);

void scheduler_init(void);
int scheduler_add_task(const char *name, task_callback callback, int interval_ticks);
void scheduler_tick(void);
void scheduler_list(void);

#endif

