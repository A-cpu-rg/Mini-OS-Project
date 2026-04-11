#ifndef KEYBOARD_H
#define KEYBOARD_H

#define MAX_INPUT 256

/* Read a full line from stdin into buf (max MAX_INPUT chars). 
   Returns number of characters read. */
int  kb_read_line(char *buf);

/* Non-blocking key check: returns char if available, 0 otherwise.
   NOTE: Requires terminal to be in raw mode (see keyboard.c) */
char kb_key_pressed(void);

/* Set terminal to raw (non-blocking) mode */
void kb_raw_mode(void);

/* Restore terminal to normal (cooked) mode */
void kb_normal_mode(void);

#endif
