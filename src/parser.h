#ifndef PARSER_H
#define PARSER_H

#define PARSER_MAX_ARGS  8
#define PARSER_MAX_LEN  64

typedef struct {
    char cmd[PARSER_MAX_LEN];
    char args[PARSER_MAX_ARGS][PARSER_MAX_LEN];
    int  argc;
} Command;

int parse_command(char *input, Command *cmd);

#endif
