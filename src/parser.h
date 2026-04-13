#ifndef PARSER_H
#define PARSER_H

#define PARSER_MAX_ARGS   10
#define PARSER_MAX_LEN   256

typedef struct {
    char  cmd [PARSER_MAX_LEN];
    char  args[PARSER_MAX_ARGS][PARSER_MAX_LEN];
    int   argc;                 /* number of arguments (not counting cmd) */
} Command;

/* Parses null-terminated input string into a Command.
   Modifies input in-place (inserts '\0' at delimiters).
   Returns 1 on success, 0 if input is empty/blank.          */
int parse_command(char *input, Command *cmd);

#endif /* PARSER_H */
