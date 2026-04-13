/*
 * scheduler.c  —  Round-Robin Process Scheduler
 * No stdio / stdlib.
 */
#include "scheduler.h"
#include "string.h"

static PCB  procs[SCHED_MAX_PROCS];
static int  next_pid   = 1;

static void safe_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (i < max - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

void sched_init(void) {
    for (int i = 0; i < SCHED_MAX_PROCS; i++) {
        procs[i].used      = 0;
        procs[i].pid       = 0;
        procs[i].state     = PROC_DONE;
        procs[i].burst     = 0;
        procs[i].remaining = 0;
        procs[i].priority  = 0;
        procs[i].name[0]   = '\0';
    }
    next_pid = 1;
}

int sched_add(const char *name, int burst, int priority) {
    for (int i = 0; i < SCHED_MAX_PROCS; i++) {
        if (!procs[i].used) {
            procs[i].pid       = next_pid++;
            procs[i].burst     = (burst > 0)    ? burst    : 5;
            procs[i].remaining = procs[i].burst;
            procs[i].priority  = (priority >= 0) ? priority : 0;
            procs[i].state     = PROC_READY;
            procs[i].used      = 1;
            safe_copy(procs[i].name, name, SCHED_NAME_LEN);
            return procs[i].pid;
        }
    }
    return -1;   /* table full */
}

/*
 * sched_tick — one round-robin pass.
 * Each READY process runs for up to TIME_QUANTUM units.
 */
void sched_tick(void) {
    for (int i = 0; i < SCHED_MAX_PROCS; i++) {
        if (!procs[i].used || procs[i].state != PROC_READY) continue;

        procs[i].state = PROC_RUNNING;

        int q = TIME_QUANTUM;
        while (q-- > 0 && procs[i].remaining > 0)
            procs[i].remaining--;

        if (procs[i].remaining <= 0) {
            procs[i].state = PROC_DONE;
            procs[i].used  = 0;           /* free slot */
        } else {
            procs[i].state = PROC_READY;
        }
    }
}

void sched_kill(int pid) {
    for (int i = 0; i < SCHED_MAX_PROCS; i++) {
        if (procs[i].used && procs[i].pid == pid) {
            procs[i].used  = 0;
            procs[i].state = PROC_DONE;
            return;
        }
    }
}

/*
 * sched_list — writes one line per active process into buf.
 * Format: "PID:<n> NAME:<s> STATE:<s> BURST:<n> REMAIN:<n> PRI:<n>\n"
 * Returns number of active processes.
 */
int sched_list(char *buf, int buf_size) {
    static const char *state_str[] = { "READY", "RUNNING", "BLOCKED", "DONE" };
    int pos   = 0;
    int count = 0;

    for (int i = 0; i < SCHED_MAX_PROCS; i++) {
        if (!procs[i].used) continue;
        if (pos >= buf_size - 80) break;

        char num[12];

        my_strcpy(buf + pos, "PID:"); pos += 4;
        my_int_to_str(procs[i].pid, num);
        my_strcpy(buf + pos, num); pos += my_strlen(num);

        my_strcpy(buf + pos, " NAME:"); pos += 6;
        my_strcpy(buf + pos, procs[i].name); pos += my_strlen(procs[i].name);

        my_strcpy(buf + pos, " STATE:"); pos += 7;
        my_strcpy(buf + pos, state_str[procs[i].state]);
        pos += my_strlen(state_str[procs[i].state]);

        my_strcpy(buf + pos, " BURST:"); pos += 7;
        my_int_to_str(procs[i].burst, num);
        my_strcpy(buf + pos, num); pos += my_strlen(num);

        my_strcpy(buf + pos, " REMAIN:"); pos += 8;
        my_int_to_str(procs[i].remaining, num);
        my_strcpy(buf + pos, num); pos += my_strlen(num);

        my_strcpy(buf + pos, " PRI:"); pos += 5;
        my_int_to_str(procs[i].priority, num);
        my_strcpy(buf + pos, num); pos += my_strlen(num);

        buf[pos++] = '\n';
        count++;
    }
    buf[pos] = '\0';
    return count;
}

int sched_count(void) {
    int c = 0;
    for (int i = 0; i < SCHED_MAX_PROCS; i++)
        if (procs[i].used) c++;
    return c;
}
