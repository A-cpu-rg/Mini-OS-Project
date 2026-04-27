  
                                                        
                   
   
#include "parser.h"
#include "string.h"

                                                               
static void rtrim(char *s) {
    int len = my_strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r' ||
                        s[len-1] == ' '  || s[len-1] == '\t')) {
        s[--len] = '\0';
    }
}

static void safe_copy(char *dst, const char *src, int max) {
    int i = 0;
    while (i < max - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

  
                                           
                                        
                                         
                                          
   
int parse_command(char *input, Command *cmd) {
    if (!input || !cmd) return 0;

    rtrim(input);
    if (input[0] == '\0') return 0;

    char *tokens[PARSER_MAX_ARGS + 1];
    int count = my_tokenize(input, tokens, PARSER_MAX_ARGS + 1, ' ');
    if (count == 0) return 0;

    safe_copy(cmd->cmd, tokens[0], PARSER_MAX_LEN);

    cmd->argc = count - 1;
    for (int i = 0; i < cmd->argc && i < PARSER_MAX_ARGS; i++)
        safe_copy(cmd->args[i], tokens[i + 1], PARSER_MAX_LEN);

    return 1;
}
