#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SCHED_MAX_PROCS  16
#define SCHED_NAME_LEN   32
#define TIME_QUANTUM      2      /* CPU time units per round-robin slice */

typedef enum {
    PROC_READY   = 0,
    PROC_RUNNING = 1,
    PROC_BLOCKED = 2,
    PROC_DONE    = 3
} ProcState;

typedef struct {
    int       pid;
    char      name[SCHED_NAME_LEN];
    ProcState state;
    int       burst;     /* Total CPU units needed          */
    int       remaining; /* Remaining CPU units             */
    int       priority;  /* Higher value = higher priority  */
    int       used;
} PCB;

void sched_init (void);
int  sched_add  (const char *name, int burst, int priority); /* returns pid or -1 */
void sched_tick (void);             /* One round-robin cycle            */
void sched_kill (int pid);
int  sched_list (char *buf, int buf_size); /* returns active count       */
int  sched_count(void);

#endif /* SCHEDULER_H */
