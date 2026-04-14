/*
 * shell.h  —  MiniOS Phase 1 Shell Interface
 */
#ifndef SHELL_H
#define SHELL_H

void shell_init(void);
int  shell_exec_line(char *raw);
void run_shell(void);

#endif /* SHELL_H */
