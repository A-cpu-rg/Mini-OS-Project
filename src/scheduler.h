#ifndef SCHEDULER_H
#define SCHEDULER_H

  
                                            
  
                                                     
   

#define SCHED_MAX_TASKS 8

typedef void (*TaskFn)(void);

typedef struct {
    int    id;
    int    active;
    TaskFn fn;
} Task;

void scheduler_init(void);
int  scheduler_add(TaskFn fn);
int  scheduler_remove(int id);
int  run_all_tasks(void);

                                   
int scheduler_counter_value(void);

#endif

