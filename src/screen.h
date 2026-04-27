#ifndef SCREEN_H
#define SCREEN_H

void screen_print(const char *s);
void screen_print_line(const char *s);
void screen_newline(void);

                                             
void screen_print_sep(const char *ch, int count);
void screen_print_title(const char *title);
void screen_print_ok(const char *msg);
void screen_print_error(const char *msg);
void screen_print_kv(const char *key, const char *value, int pad_to);
void screen_print_kv_int(const char *key, int value, int pad_to);

#endif
