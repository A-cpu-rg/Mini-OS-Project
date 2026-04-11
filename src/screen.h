#ifndef SCREEN_H
#define SCREEN_H

void screen_clear(void);
void screen_move_cursor(int row, int col);
void screen_print(const char *s);
void screen_print_char(char c);
void screen_print_int(int n);
void screen_newline(void);
void screen_print_line(const char *s);   /* print + newline */
void screen_print_banner(void);          /* MiniOS welcome banner */

#endif
