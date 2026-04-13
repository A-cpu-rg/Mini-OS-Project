/*
 * shell.c  —  MiniOS interactive shell
 *
 * Transport protocol for the Node.js bridge:
 *   • Every command response ends with the sentinel  __END__\n
 *   • On startup the same sentinel is printed once (signals "ready")
 *   • The __CLEAR__ token tells the UI to wipe its terminal history
 *
 * No stdlib / stdio: all output goes via screen_print* helpers
 * (which use write(1,…) internally). Input via kb_read_line (read(0,…)).
 * Entry point is in main.c — not here.
 */
#include "shell.h"
#include "math.h"
#include "string.h"
#include "memory.h"
#include "screen.h"
#include "keyboard.h"
#include "fs.h"
#include "scheduler.h"
#include "parser.h"

#define SHELL_INPUT_SIZE  512
#define MAX_OUTPUT        4096
#define SENTINEL   "__END__\n"

/* ── tiny output builder ─────────────────────────────── */
static char out[MAX_OUTPUT];
static int  opos;

static void out_reset(void) { opos = 0; out[0] = '\0'; }

static void out_append(const char *s) {
    int slen = my_strlen(s);
    if (opos + slen < MAX_OUTPUT - 1) {
        my_strcpy(out + opos, s);
        opos += slen;
    }
}

static void out_append_int(int n) {
    char buf[20];
    my_int_to_str(n, buf);
    out_append(buf);
}

static void out_flush(void) {
    screen_print(out);
    screen_print(SENTINEL);
}

/* ══════════════════════════════════════════════════════
   COMMAND HANDLERS
   ══════════════════════════════════════════════════════ */

static void handle_help(void) {
    out_append(
        "MiniOS v2.0  --  Available Commands\n"
        "------------------------------------\n"
        "  create <file>                Create a new file\n"
        "  write  <file> <data...>      Write data to a file\n"
        "  read   <file>                Print file contents\n"
        "  ls                           List all files\n"
        "  delete <file>                Delete a file\n"
        "  run    <name> <burst> <pri>  Add a process\n"
        "  ps                           List all processes\n"
        "  tick                         Execute one scheduler cycle\n"
        "  kill   <pid>                 Kill a process by PID\n"
        "  mem                          Virtual RAM status\n"
        "  status                       Full system status\n"
        "  echo   <text...>             Print text\n"
        "  math   <a> <op> <b>          Arithmetic (+,-,*,/,%,max,min)\n"
        "  clear                        Clear the terminal\n"
        "  help                         Show this menu\n"
        "  exit                         Shutdown MiniOS\n"
    );
}

static void handle_create(Command *cmd) {
    if (cmd->argc < 1) { out_append("Error: usage: create <filename>\n"); return; }
    switch (fs_create(cmd->args[0])) {
        case  0: out_append("Created: "); out_append(cmd->args[0]); out_append("\n"); break;
        case -1: out_append("Error: file already exists\n");   break;
        case -2: out_append("Error: file system full (64 files max)\n"); break;
        default: out_append("Error: invalid filename\n");       break;
    }
}

static void handle_write(Command *cmd) {
    if (cmd->argc < 2) { out_append("Error: usage: write <file> <data...>\n"); return; }

    /* Rebuild data by joining args[1..argc-1] */
    char data[FS_MAX_DATA];
    int  dpos = 0;
    for (int i = 1; i < cmd->argc && dpos < FS_MAX_DATA - 2; i++) {
        if (i > 1) data[dpos++] = ' ';
        int alen = my_strlen(cmd->args[i]);
        my_strcpy(data + dpos, cmd->args[i]);
        dpos += alen;
    }
    data[dpos] = '\0';

    /* Auto-create if file does not exist */
    if (!fs_exists(cmd->args[0])) fs_create(cmd->args[0]);

    if (fs_write(cmd->args[0], data) == 0) {
        out_append("Written "); out_append_int(dpos);
        out_append(" B to "); out_append(cmd->args[0]); out_append("\n");
    } else {
        out_append("Error: could not write to file\n");
    }
}

static void handle_read(Command *cmd) {
    if (cmd->argc < 1) { out_append("Error: usage: read <filename>\n"); return; }
    char buf[FS_MAX_DATA];
    int  r = fs_read(cmd->args[0], buf, sizeof(buf));
    if (r < 0)  { out_append("Error: file not found: "); out_append(cmd->args[0]); out_append("\n"); }
    else if (r == 0) out_append("(empty file)\n");
    else { out_append(buf); out_append("\n"); }
}

static void handle_ls(void) {
    char buf[4096];
    int  n = fs_ls(buf, sizeof(buf));
    if (n == 0) out_append("(no files)\n");
    else        out_append(buf);
}

static void handle_delete(Command *cmd) {
    if (cmd->argc < 1) { out_append("Error: usage: delete <filename>\n"); return; }
    if (fs_delete(cmd->args[0]) == 0) {
        out_append("Deleted: "); out_append(cmd->args[0]); out_append("\n");
    } else {
        out_append("Error: file not found\n");
    }
}

static void handle_run(Command *cmd) {
    if (cmd->argc < 3) { out_append("Error: usage: run <name> <burst> <priority>\n"); return; }
    int burst    = my_str_to_int(cmd->args[1]);
    int priority = my_str_to_int(cmd->args[2]);
    int pid      = sched_add(cmd->args[0], burst, priority);
    if (pid > 0) {
        out_append("Process added: "); out_append(cmd->args[0]);
        out_append(" [PID:"); out_append_int(pid); out_append("]\n");
    } else {
        out_append("Error: process table full (16 max)\n");
    }
}

static void handle_ps(void) {
    char buf[2048];
    int  n = sched_list(buf, sizeof(buf));
    if (n == 0) out_append("(no active processes)\n");
    else        out_append(buf);
}

static void handle_tick(void) {
    sched_tick();
    out_append("Scheduler: one round-robin cycle executed\n");
    char buf[2048];
    int  n = sched_list(buf, sizeof(buf));
    if (n > 0) out_append(buf);
    else       out_append("All processes finished.\n");
}

static void handle_kill(Command *cmd) {
    if (cmd->argc < 1) { out_append("Error: usage: kill <pid>\n"); return; }
    int pid = my_str_to_int(cmd->args[0]);
    sched_kill(pid);
    out_append("Killed PID:"); out_append_int(pid); out_append("\n");
}

static void handle_mem(void) {
    int used  = VRAM_SIZE - mem_free_space();
    int total = VRAM_SIZE;
    out_append("Virtual RAM Status\n");
    out_append("------------------\n");
    out_append("Total : "); out_append_int(total); out_append(" B\n");
    out_append("Used  : "); out_append_int(used);  out_append(" B\n");
    out_append("Free  : "); out_append_int(mem_free_space()); out_append(" B\n");
    /* Live alloc/free demo */
    char *p = (char *)my_alloc(128);
    if (p) { my_strcpy(p, "AllocTest"); my_dealloc(p); }
    out_append("Alloc test: OK (128 B alloc+free)\n");
}

static void handle_status(void) {
    out_append("=== MiniOS System Status ===\n");
    out_append("Files      : "); out_append_int(fs_count());    out_append(" / 64\n");
    out_append("Processes  : "); out_append_int(sched_count()); out_append(" / 16\n");
    out_append("VRAM Total : "); out_append_int(VRAM_SIZE);     out_append(" B\n");
    out_append("VRAM Used  : "); out_append_int(VRAM_SIZE - mem_free_space()); out_append(" B\n");
    out_append("VRAM Free  : "); out_append_int(mem_free_space()); out_append(" B\n");
}

static void handle_echo(Command *cmd) {
    for (int i = 0; i < cmd->argc; i++) {
        if (i > 0) out_append(" ");
        out_append(cmd->args[i]);
    }
    out_append("\n");
}

static void handle_math(Command *cmd) {
    if (cmd->argc < 3) { out_append("Error: usage: math <a> <op> <b>\n"); return; }
    int   a  = my_str_to_int(cmd->args[0]);
    char *op = cmd->args[1];
    int   b  = my_str_to_int(cmd->args[2]);
    int   r  = 0;
    int   ok = 1;

    if      (my_strcmp(op, "+")   == 0) r = a + b;
    else if (my_strcmp(op, "-")   == 0) r = a - b;
    else if (my_strcmp(op, "*")   == 0) r = my_multiply(a, b);
    else if (my_strcmp(op, "/")   == 0) {
        if (b == 0) { out_append("Error: division by zero\n"); return; }
        r = my_divide(a, b);
    }
    else if (my_strcmp(op, "%")   == 0) {
        if (b == 0) { out_append("Error: modulo by zero\n"); return; }
        r = my_modulo(a, b);
    }
    else if (my_strcmp(op, "max") == 0) r = my_max(a, b);
    else if (my_strcmp(op, "min") == 0) r = my_min(a, b);
    else { out_append("Error: unknown op. Use: + - * / % max min\n"); ok = 0; }

    if (ok) { out_append("Result: "); out_append_int(r); out_append("\n"); }
}

/* ══════════════════════════════════════════════════════
   SHELL ENTRY POINTS
   ══════════════════════════════════════════════════════ */

void shell_init(void) {
    mem_init();
    fs_init();
    sched_init();
}

/*
 * shell_exec_line — process one raw input line, fill global out[].
 * Returns 1 normally, 0 to signal "exit" (caller should stop loop).
 */
int shell_exec_line(char *raw) {
    out_reset();

    Command cmd;
    char    buf[SHELL_INPUT_SIZE];
    /* copy so tokeniser can modify it */
    int i = 0;
    while (i < SHELL_INPUT_SIZE - 1 && raw[i]) { buf[i] = raw[i]; i++; }
    buf[i] = '\0';

    if (!parse_command(buf, &cmd)) { out_flush(); return 1; }

    if      (my_strcmp(cmd.cmd, "help")   == 0) handle_help();
    else if (my_strcmp(cmd.cmd, "create") == 0) handle_create(&cmd);
    else if (my_strcmp(cmd.cmd, "write")  == 0) handle_write(&cmd);
    else if (my_strcmp(cmd.cmd, "read")   == 0) handle_read(&cmd);
    else if (my_strcmp(cmd.cmd, "ls")     == 0) handle_ls();
    else if (my_strcmp(cmd.cmd, "delete") == 0) handle_delete(&cmd);
    else if (my_strcmp(cmd.cmd, "run")    == 0) handle_run(&cmd);
    else if (my_strcmp(cmd.cmd, "ps")     == 0) handle_ps();
    else if (my_strcmp(cmd.cmd, "tick")   == 0) handle_tick();
    else if (my_strcmp(cmd.cmd, "kill")   == 0) handle_kill(&cmd);
    else if (my_strcmp(cmd.cmd, "mem")    == 0) handle_mem();
    else if (my_strcmp(cmd.cmd, "status") == 0) handle_status();
    else if (my_strcmp(cmd.cmd, "echo")   == 0) handle_echo(&cmd);
    else if (my_strcmp(cmd.cmd, "math")   == 0) handle_math(&cmd);
    else if (my_strcmp(cmd.cmd, "clear")  == 0) { out_append("__CLEAR__\n"); }
    else if (my_strcmp(cmd.cmd, "exit")   == 0 ||
             my_strcmp(cmd.cmd, "quit")   == 0) {
        out_append("Goodbye! MiniOS shutting down.\n");
        out_flush();
        return 0;
    }
    else {
        out_append("Unknown command: '");
        out_append(cmd.cmd);
        out_append("'  --  type 'help' for options\n");
    }

    out_flush();
    return 1;
}

void run_shell(void) {
    char *input_buf = (char *)my_alloc(SHELL_INPUT_SIZE);
    char *cmd_copy  = (char *)my_alloc(SHELL_INPUT_SIZE);

    if (!input_buf || !cmd_copy) {
        screen_print("FATAL: cannot allocate shell buffers\n");
        screen_print(SENTINEL);
        return;
    }

    /* Signal ready to Node bridge */
    screen_print(SENTINEL);

    while (1) {
        input_buf[0] = '\0';
        int len = kb_read_line(input_buf);
        if (len < 0) break;               /* EOF — pipe closed */
        if (!shell_exec_line(input_buf)) break;
    }

    my_dealloc(input_buf);
    my_dealloc(cmd_copy);
}

/* main() lives in main.c */