/*
 * keyboard.c — Input using read() syscall (no scanf)
 */
#include "keyboard.h"
#include <unistd.h> 

/*
 * kb_read_line — reads one line from stdin into buf.
 * Stops at newline or EOF. Strips the trailing newline.
 * Returns number of characters read, or -1 on EOF with empty input.
 */
int kb_read_line(char *buf) {
    int i = 0;
    char c;
    while (i < MAX_INPUT - 1) {
        int r = (int)read(STDIN_FILENO, &c, 1);
        if (r <= 0) {
            if (i == 0) return -1;  /* EOF, nothing read */
            break;
        }
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}
