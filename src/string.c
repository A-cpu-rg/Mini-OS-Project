/*
 * string.c — Custom string library (no <string.h>)
 */
#include "string.h"

int my_strlen(const char *s) {
    int i = 0;
    while (s[i] != '\0') i++;  /* count chars until null terminator */
    return i;
}

void my_strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') { dest[i] = src[i]; i++; } /* copy chars until null terminator */
    dest[i] = '\0';  /* add null terminator at the end */
}

int my_strcmp(const char *a, const char *b) {
    int i = 0; /* compare chars until null terminator of either string */
    while (a[i] != '\0' && b[i] != '\0') { /* compare chars at position i */
        if (a[i] != b[i]) return a[i] - b[i]; /* if chars differ, return their difference */
        i++; /* move to next char */
    }
    return a[i] - b[i]; /* if we reached the end of one string, return difference of null terminator and next char */
}

/*
 * my_tokenize — splits input by delim in-place.
 * Writes pointers into tokens[]. Returns token count.
 * WARNING: modifies input string.
 */
int my_tokenize(char *input, char *tokens[], int max, char delim) { /* split input into tokens by delim, store pointers in tokens[], return count */
    int count = 0;
    int i = 0;
    int len = my_strlen(input);

    while (i < len && input[i] == delim) i++;   /* skip leading */

    while (i < len && count < max) { /* while we have more input and space for tokens */
        tokens[count++] = &input[i];
        while (i < len && input[i] != delim) i++; /* move i to next delim or end */
        if (i < len) { input[i] = '\0'; i++; }    
        while (i < len && input[i] == delim) i++;  /* skip consecutive */
    }
    return count;
}

void my_int_to_str(int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[20];
    int i = 0, neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    while (n > 0) { tmp[i++] = '0' + (n % 10); n /= 10; }
    if (neg) tmp[i++] = '-';
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

int my_str_to_int(const char *s) {
    int result = 0, sign = 1, i = 0;
    if (s[i] == '-') { sign = -1; i++; }
    while (s[i] >= '0' && s[i] <= '9') {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    return sign * result;
}
