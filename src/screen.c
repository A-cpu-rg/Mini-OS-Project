#include "screen.h"
#include "string.h"
#include <stdio.h>

/* Clear terminal using ANSI escape code */
void screen_clear(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

/* Move terminal cursor to (row, col) — 1-indexed */
void screen_move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
    fflush(stdout);
}

/* Print a string */
void screen_print(const char *s) {
    int i = 0;
    while (s[i] != '\0') { putchar(s[i]); i++; }
    fflush(stdout);
}

/* Print a single character */
void screen_print_char(char c) {
    putchar(c);
    fflush(stdout);
}

/* Print an integer */
void screen_print_int(int n) {
    /* Inline int-to-str to avoid circular deps */
    if (n == 0) { putchar('0'); fflush(stdout); return; }
    int neg = 0; int i = 0; char tmp[20];
    if (n < 0) { neg = 1; n = -n; }
    while (n > 0) { tmp[i++] = '0' + (n % 10); n /= 10; }
    if (neg) tmp[i++] = '-';
    while (i > 0) putchar(tmp[--i]);
    fflush(stdout);
}

/* Print newline */
void screen_newline(void) {
    putchar('\n');
    fflush(stdout);
}

/* Print string followed by newline */
void screen_print_line(const char *s) {
    screen_print(s);
    screen_newline();
}

/* Welcome banner for MiniOS */
void screen_print_banner(void) {
    screen_clear();
    screen_print_line("╔══════════════════════════════════════╗");
    screen_print_line("║         MiniOS v1.0  (Phase 1)       ║");
    screen_print_line("║     Team: TeamHamza | C from scratch  ║");
    screen_print_line("╚══════════════════════════════════════╝");
    screen_print_line("Type 'help' to see available commands.");
    screen_newline();
}
