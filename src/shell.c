  
                                   
  
                                                           
                                          
                                           
   
#include "shell.h"
#include "math.h"
#include "string.h"
#include "screen.h"
#include "keyboard.h"
#include "parser.h"
#include "filesystem.h"
#include "scheduler.h"
#include "memory.h"
#include <unistd.h>

#define SHELL_INPUT_SIZE 256
#define SHELL_PROMPT_PATH "/home"
#define SHELL_TITLE "MiniOS v2.0"

static void shell_print_prompt(void);

static void shell_print_ok_file(const char *prefix, const char *name, const char *suffix) {
    screen_print("[OK] ");
    screen_print(prefix);
    screen_print(" '");
    screen_print(name);
    screen_print("'");
    screen_print_line(suffix);
}

static void shell_print_prompt(void) {
    screen_print("miniOS:");
    screen_print(SHELL_PROMPT_PATH);
    screen_print("$ ");
}

                                                              

static void handle_help(void) {
    screen_print_title(SHELL_TITLE " - Commands");
    screen_print_line("help                 Show this menu");
    screen_print_line("echo <text...>       Print text");
    screen_print_line("math <a> <op> <b>    Arithmetic: op is + - * /");
    screen_print_line("touch <file>         Create file (if missing)");
    screen_print_line("write <file> <data>  Write data to file (overwrites)");
    screen_print_line("read <file>          Read file contents");
    screen_print_line("ls                   List files");
    screen_print_line("delete <file>        Delete file");
    screen_print_line("mem                  Show memory status");
    screen_print_line("run                  Run one scheduler tick (demo)");
    screen_print_line("clear                Clear screen + show boot banner");
    screen_print_line("exit                 Quit MiniOS");
    screen_print_sep("-", 32);
}

static void handle_echo(Command *cmd) {
    for (int i = 0; i < cmd->argc; i++) {
        if (i > 0) screen_print(" ");
        screen_print(cmd->args[i]);
    }
    screen_newline();
}

static void handle_math(Command *cmd) {
    if (cmd->argc < 3) {
        screen_print_error("Usage: math <a> <op> <b>   (op: + - * /)");
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
        if (b == 0) { screen_print_error("division by zero"); return; }
        r = my_divide(a, b);
    } else {
        screen_print_error("unknown operator (use + - * /)");
        return;
    }

    screen_print_title("Math Result");
    screen_print_kv_int("A", a, 6);
    screen_print_kv("Op", op, 6);
    screen_print_kv_int("B", b, 6);
    screen_print_sep("-", 16);
    screen_print_kv_int("Result", r, 6);
}

static void handle_touch(Command *cmd) {
    if (cmd->argc < 1) { screen_print_error("Usage: touch <filename>"); return; }
    if (fs_touch(cmd->args[0])) {
        shell_print_ok_file("File", cmd->args[0], " created");
    } else {
        screen_print_error("cannot create file");
    }
}

static void handle_delete(Command *cmd) {
    if (cmd->argc < 1) { screen_print_error("Usage: delete <filename>"); return; }
    if (fs_delete(cmd->args[0])) {
        shell_print_ok_file("File", cmd->args[0], " deleted");
    } else {
        screen_print_error("file not found");
    }
}

static void handle_ls(void) {
    char names[FS_MAX_FILES][FS_NAME_MAX];
    unsigned int sizes[FS_MAX_FILES];
    int n = fs_list_detailed(names, sizes, FS_MAX_FILES);
    screen_print_title("Files in Virtual System");
    if (n <= 0) { screen_print_line("(empty)"); screen_print_sep("-", 32); return; }

    for (int i = 0; i < n; i++) {
        char num[16];
        char sz[32];
        my_int_to_str(i + 1, num);
        my_int_to_str((int)sizes[i], sz);

        screen_print("[");
        screen_print(num);
        screen_print("] ");
        screen_print(names[i]);

                                                 
        int pad = my_clamp(12 - my_strlen(names[i]), 1, 12);
        for (int k = 0; k < pad; k++) screen_print(" ");

        screen_print("(");
        screen_print(sz);
        screen_print_line(" bytes)");
    }
    screen_print_sep("-", 32);
}

static void build_rest_args(Command *cmd, int start_idx, char *out_buf, int out_max) {
    int pos = 0;
    if (!out_buf || out_max <= 1) return;
    out_buf[0] = '\0';
    for (int i = start_idx; i < cmd->argc; i++) {
        char *s = cmd->args[i];
        if (!s) continue;
        if (i > start_idx && pos < out_max - 1) out_buf[pos++] = ' ';
        for (int j = 0; s[j] && pos < out_max - 1; j++) out_buf[pos++] = s[j];
    }
    out_buf[pos] = '\0';
}

static void handle_write(Command *cmd) {
    if (cmd->argc < 2) { screen_print_error("Usage: write <filename> <data...>"); return; }
    char data_buf[256];
    build_rest_args(cmd, 1, data_buf, 256);
    if (fs_write(cmd->args[0], data_buf)) {
        shell_print_ok_file("Data written to", cmd->args[0], "");
    } else {
        screen_print_error("write failed (out of RAM / invalid name)");
    }
}

static void handle_read(Command *cmd) {
    if (cmd->argc < 1) { screen_print_error("Usage: read <filename>"); return; }
    const char *data = 0;
    unsigned int sz = 0;
    if (!fs_read(cmd->args[0], &data, &sz)) { screen_print_error("File not found"); return; }

    screen_print("----- FILE: ");
    screen_print(cmd->args[0]);
    screen_print_line(" -----");
    screen_print_line(data);
    screen_print_sep("-", 24);
}

static void handle_run(void) {
    int ran = run_all_tasks();
    screen_print_ok("Scheduler tick executed");
    screen_print_kv_int("Tasks ran", ran, 10);
    screen_print_kv_int("Counter", scheduler_counter_value(), 10);
}

static void handle_mem(void) {
    screen_print_title("Memory Status");
    screen_print_kv("Total", "1 MB", 6);
    screen_print_kv_int("Used", (int)memory_bytes_used(), 6);
    screen_print_kv_int("Free", (int)memory_bytes_free(), 6);
    screen_print_sep("-", 32);
}

static void shell_boot_banner(void) {
    screen_clear();
    screen_set_cursor(1, 1);
    screen_print_title(SHELL_TITLE " Loaded");
    screen_print_line("Type 'help' to begin");
    screen_newline();
}

                                                              

void shell_init(void) {
    shell_boot_banner();

    memory_init();
    fs_init();
    scheduler_init();
}

  
                                                
                                        
   
int shell_exec_line(char *raw) {
    Command cmd;
    char    buf[SHELL_INPUT_SIZE];

    int i = 0;
    while (i < SHELL_INPUT_SIZE - 1 && raw[i]) { buf[i] = raw[i]; i++; }
    buf[i] = '\0';

    if (!parse_command(buf, &cmd)) { return 1; }

    if      (my_strcmp(cmd.cmd, "help")  == 0) handle_help();
    else if (my_strcmp(cmd.cmd, "echo")  == 0) handle_echo(&cmd);
    else if (my_strcmp(cmd.cmd, "math")  == 0) handle_math(&cmd);
    else if (my_strcmp(cmd.cmd, "touch") == 0) handle_touch(&cmd);
    else if (my_strcmp(cmd.cmd, "write") == 0) handle_write(&cmd);
    else if (my_strcmp(cmd.cmd, "read")  == 0) handle_read(&cmd);
    else if (my_strcmp(cmd.cmd, "ls")    == 0) handle_ls();
    else if (my_strcmp(cmd.cmd, "delete") == 0) handle_delete(&cmd);
    else if (my_strcmp(cmd.cmd, "mem")   == 0) handle_mem();
    else if (my_strcmp(cmd.cmd, "run")   == 0) handle_run();
    else if (my_strcmp(cmd.cmd, "clear") == 0) {
        shell_boot_banner();
        return 1;
    }
    else if (my_strcmp(cmd.cmd, "exit") == 0 ||
             my_strcmp(cmd.cmd, "quit") == 0) {
        screen_print_ok("Goodbye!");
        return 0;
    }
    else {
        screen_print("[ERROR] Unknown command: '");
        screen_print(cmd.cmd);
        screen_print_line("'  (type 'help')");
    }
    return 1;
}

void run_shell(void) {
    char input_buf[SHELL_INPUT_SIZE];
    int  prompt_shown = 0;

    kb_enable_nonblocking();

    while (1) {
        run_all_tasks();

        if (!prompt_shown) {
            shell_print_prompt();
            prompt_shown = 1;
        }

        if (!keyPressed()) {
            usleep(10000);
            continue;
        }

        int len = kb_poll_line(input_buf, SHELL_INPUT_SIZE);
        if (len < 0) break;              
        if (len == 0) {                           
            usleep(10000);                             
            continue;
        }
        prompt_shown = 0;
        if (!shell_exec_line(input_buf)) break;
    }
}
