  
                                                      
                   
   
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

void screen_putc(char c) {
    write(STDOUT_FILENO, &c, 1);
}

void screen_print_line(const char *s) {
    screen_print(s);
    screen_newline();
}

void screen_clear(void) {
    screen_print("\033[2J");
}

void screen_set_cursor(int x, int y) {
    char buf[32];
    int i = 0;

    if (x < 1) x = 1;
    if (y < 1) y = 1;

    buf[i++] = '\033';
    buf[i++] = '[';

    if (y >= 100) {
        buf[i++] = (char)('0' + (y / 100) % 10);
    }
    if (y >= 10) {
        buf[i++] = (char)('0' + (y / 10) % 10);
    }
    buf[i++] = (char)('0' + (y % 10));
    buf[i++] = ';';

    if (x >= 100) {
        buf[i++] = (char)('0' + (x / 100) % 10);
    }
    if (x >= 10) {
        buf[i++] = (char)('0' + (x / 10) % 10);
    }
    buf[i++] = (char)('0' + (x % 10));
    buf[i++] = 'H';
    write(STDOUT_FILENO, buf, i);
}

void screen_print_sep(const char *ch, int count) {
    if (!ch || ch[0] == '\0' || count <= 0) return;
    for (int i = 0; i < count; i++) screen_putc(ch[0]);
    screen_newline();
}

void screen_print_title(const char *title) {
    screen_newline();
    screen_print("## ");
    screen_print(title ? title : "");
    screen_newline();
    screen_print_sep("-", 32);
}

static void screen_print_tag(const char *tag) {
    screen_print("[");
    screen_print(tag);
    screen_print("] ");
}

void screen_print_ok(const char *msg) {
    screen_print_tag("OK");
    screen_print_line(msg ? msg : "");
}

void screen_print_error(const char *msg) {
    screen_print_tag("ERROR");
    screen_print_line(msg ? msg : "");
}

void screen_print_kv(const char *key, const char *value, int pad_to) {
    int klen = key ? my_strlen(key) : 0;
    screen_print(key ? key : "");
    if (pad_to < klen) pad_to = klen;
    for (int i = klen; i < pad_to; i++) screen_print(" ");
    screen_print(": ");
    screen_print_line(value ? value : "");
}

void screen_print_kv_int(const char *key, int value, int pad_to) {
    char buf[32];
    my_int_to_str(value, buf);
    screen_print_kv(key, buf, pad_to);
}
