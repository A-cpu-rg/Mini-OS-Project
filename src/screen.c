/*
 * screen.c — Output using write() syscall (no printf)
 * Phase 1: Mini OS
 */
#include "screen.h"
#include "string.h"
#include <unistd.h>

void screen_print(const char *s) {
    int len = my_strlen(s);
    write(STDOUT_FILENO, s, len);
}

void screen_newline(void) {
    write(STDOUT_FILENO, "\n", 1);
}

void screen_print_line(const char *s) {
    screen_print(s);
    screen_newline();
}
