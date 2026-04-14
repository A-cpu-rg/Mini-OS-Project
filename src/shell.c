/*
 * shell.c  —  MiniOS Phase 1 Shell
 *
 * Supported commands: help, echo, clear, math <a> <op> <b>
 * Input  : keyboard module (read syscall)
 * Output : screen module   (write syscall)
 */
#include "shell.h"
#include "math.h"
#include "string.h"
#include "screen.h"
#include "keyboard.h"
#include "parser.h"

#define SHELL_INPUT_SIZE 256

/* ── output buffer ─────────────────────────────────────── */
static char out[1024];
static int  opos;

static void out_reset(void)          { opos = 0; out[0] = '\0'; }

static void out_append(const char *s) {
    int slen = my_strlen(s);
    if (opos + slen < 1023) {
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
}

/* ── command handlers ──────────────────────────────────── */

static void handle_help(void) {
    out_append(
        "MiniOS Phase 1  --  Available Commands\n"
        "--------------------------------------\n"
        "  help              Show this menu\n"
        "  echo <text...>    Print text to screen\n"
        "  math <a> <op> <b> Arithmetic: op is + - * /\n"
        "  clear             Clear the terminal\n"
        "  exit              Quit MiniOS\n"
    );
}

static void handle_echo(Command *cmd) {
    for (int i = 0; i < cmd->argc; i++) {
        if (i > 0) out_append(" ");
        out_append(cmd->args[i]);
    }
    out_append("\n");
}

static void handle_math(Command *cmd) {
    if (cmd->argc < 3) {
        out_append("Usage: math <a> <op> <b>   (op: + - * /)\n");
        return;
    }
    int   a  = my_str_to_int(cmd->args[0]);
    char *op = cmd->args[1];
    int   b  = my_str_to_int(cmd->args[2]);
    int   r  = 0;

    if (my_strcmp(op, "+") == 0) {
        r = a + b;
    } else if (my_strcmp(op, "-") == 0) {
        r = a - b;
    } else if (my_strcmp(op, "*") == 0) {
        r = my_multiply(a, b);
    } else if (my_strcmp(op, "/") == 0) {
        if (b == 0) { out_append("Error: division by zero\n"); return; }
        r = my_divide(a, b);
    } else {
        out_append("Error: unknown operator. Use + - * /\n");
        return;
    }

    out_append_int(a);
    out_append(" ");
    out_append(op);
    out_append(" ");
    out_append_int(b);
    out_append(" = ");
    out_append_int(r);
    out_append("\n");
}

/* ── shell entry points ────────────────────────────────── */

void shell_init(void) {
    screen_print("MiniOS Phase 1 — Shell Ready\n");
    screen_print("Type 'help' for available commands.\n\n");
}

/*
 * shell_exec_line — process one raw input line.
 * Returns 1 normally, 0 to signal exit.
 */
int shell_exec_line(char *raw) {
    out_reset();

    Command cmd;
    char    buf[SHELL_INPUT_SIZE];

    int i = 0;
    while (i < SHELL_INPUT_SIZE - 1 && raw[i]) { buf[i] = raw[i]; i++; }
    buf[i] = '\0';

    if (!parse_command(buf, &cmd)) { return 1; }

    screen_print("$ ");
    screen_print(raw);
    screen_print("\n");

    if      (my_strcmp(cmd.cmd, "help")  == 0) handle_help();
    else if (my_strcmp(cmd.cmd, "echo")  == 0) handle_echo(&cmd);
    else if (my_strcmp(cmd.cmd, "math")  == 0) handle_math(&cmd);
    else if (my_strcmp(cmd.cmd, "clear") == 0) {
        /* ANSI clear screen */
        screen_print("\033[2J\033[H");
        return 1;
    }
    else if (my_strcmp(cmd.cmd, "exit") == 0 ||
             my_strcmp(cmd.cmd, "quit") == 0) {
        screen_print("Goodbye!\n");
        return 0;
    }
    else {
        out_append("Unknown command: '");
        out_append(cmd.cmd);
        out_append("'  --  type 'help'\n");
    }

    out_flush();
    return 1;
}

void run_shell(void) {
    char input_buf[SHELL_INPUT_SIZE];

    while (1) {
        screen_print("miniOS> ");
        int len = kb_read_line(input_buf);
        if (len < 0) break;            /* EOF */
        if (len == 0) continue;        /* empty line */
        if (!shell_exec_line(input_buf)) break;
    }
}