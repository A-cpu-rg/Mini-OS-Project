#ifndef STRING_H
#define STRING_H

int   my_strlen(const char *s);
void  my_strcpy(char *dest, const char *src);
int   my_strcmp(const char *a, const char *b);
int   my_tokenize(char *input, char *tokens[], int max, char delim);
void  my_int_to_str(int n, char *buf);
int   my_str_to_int(const char *s);

#endif
