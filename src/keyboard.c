#include "keyboard.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios orig_termios;

/* Save original terminal settings and switch to raw mode */
void kb_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);  /* No line buffer, no echo */
    raw.c_cc[VMIN]  = 0;              /* Non-blocking */
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

/* Restore original terminal settings */
void kb_normal_mode(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

/* Read a full line (blocking). Strips trailing newline. */
int kb_read_line(char *buf) {
    int i = 0;
    int c;
    while (i < MAX_INPUT - 1) {
        c = getchar();
        if (c == EOF || c == '\n') break;
        buf[i++] = (char)c;
    }
    buf[i] = '\0';
    return i;
}

/* Non-blocking key check — returns 0 if no key available */
char kb_key_pressed(void) {
    char c = 0;
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    int r = (int)read(STDIN_FILENO, &c, 1);
    fcntl(STDIN_FILENO, F_SETFL, flags);  /* Restore */
    return r == 1 ? c : 0;
}
