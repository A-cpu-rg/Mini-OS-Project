/*
 * main.c  —  MiniOS Entry Point
 *
 * Keeps main() separate from shell.c for clean modularity.
 * Initialises all subsystems then hands control to the shell loop.
 */
#include "shell.h"

int main(void) {
    shell_init();
    run_shell();
    return 0;
}
