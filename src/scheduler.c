  
                                            
   
#include "scheduler.h"

static Task tasks[SCHED_MAX_TASKS];
static int  next_id = 1;
static int  inited = 0;

                             
static int bg_counter = 0;

static void counter_task(void) {
                                                                 
    bg_counter++;
}

static void clear_tasks(void) {
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        tasks[i].id = 0;
        tasks[i].active = 0;
        tasks[i].fn = (TaskFn)0;
    }
}

void scheduler_init(void) {
    clear_tasks();
    next_id = 1;
    bg_counter = 0;
    inited = 1;

                                          
    scheduler_add(counter_task);
}

int scheduler_add(TaskFn fn) {
    if (!inited) scheduler_init();
    if (!fn) return -1;

    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        if (!tasks[i].active) {
            tasks[i].id = next_id++;
            tasks[i].active = 1;
            tasks[i].fn = fn;
            return tasks[i].id;
        }
    }
    return -1;
}

int scheduler_remove(int id) {
    if (!inited) scheduler_init();
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        if (tasks[i].active && tasks[i].id == id) {
            tasks[i].active = 0;
            tasks[i].id = 0;
            tasks[i].fn = (TaskFn)0;
            return 1;
        }
    }
    return 0;
}

int run_all_tasks(void) {
    if (!inited) scheduler_init();
    int ran = 0;
    for (int i = 0; i < SCHED_MAX_TASKS; i++) {
        if (tasks[i].active && tasks[i].fn) {
            tasks[i].fn();
            ran++;
        }
    }
    return ran;
}

int scheduler_counter_value(void) {
    return bg_counter;
}

