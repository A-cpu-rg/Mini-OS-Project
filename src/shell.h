/*
 * shell.h  —  MiniOS Shell Public Interface
 */
#ifndef SHELL_H
#define SHELL_H

/* Initialise all subsystems (mem, fs, scheduler) */
void shell_init(void);

/* Process one raw command line; returns 0 to signal exit */
int  shell_exec_line(char *raw);

/* Main shell REPL loop — blocks until EOF or "exit" */
void run_shell(void);

#endif /* SHELL_H */
