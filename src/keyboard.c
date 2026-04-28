  
                                                     
   
#include "keyboard.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

                                                 
static char kb_linebuf[MAX_INPUT];//variables
static int  kb_pos = 0;

//before user enters,reads full line
static int read_line_blocking(char *buf) {
    int i = 0;
    char c;
    while (i < MAX_INPUT - 1) {
        int r = (int)read(STDIN_FILENO, &c, 1);
        if (r <= 0) {
            if (i == 0) return -1;                         
            break;
        }
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}
//wrapper functions
int kb_read_line(char *buf) {
    return read_line_blocking(buf);
}

int readLine(char *buf) {
    return read_line_blocking(buf);
}
//makes i/p non-blocking
int kb_enable_nonblocking(void) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (flags < 0) return 0;
    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) < 0) return 0;
    return 1;
}
//select()
int keyPressed(void) {
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    return select(STDIN_FILENO + 1, &readfds, (fd_set *)0, (fd_set *)0, &tv) > 0;
}
//reads char one-one,buffer,returns full line
int kb_poll_line(char *out, int out_max) {
    if (!out || out_max <= 1) return 0;

    while (1) {
        char c;
        ssize_t r = read(STDIN_FILENO, &c, 1);
        if (r == 0) {
            return -1;          
        }
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            return -1;
        }

        if (c == '\n') {
            int len = kb_pos;
            if (len > out_max - 1) len = out_max - 1;
            for (int i = 0; i < len; i++) out[i] = kb_linebuf[i];
            out[len] = '\0';
            kb_pos = 0;
            return len;
        }

        if (kb_pos < MAX_INPUT - 1) {
            kb_linebuf[kb_pos++] = c;
        }
                                                  
    }

    return 0;
}
//typing..?
int kb_input_in_progress(void) {
    return (kb_pos > 0) ? 1 : 0;
}
