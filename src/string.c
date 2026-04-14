/*
 * string.c — Custom string library (no <string.h>)
 * Phase 1: Mini OS
 */
#include "string.h"

int my_strlen(const char *s) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

void my_strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') { dest[i] = src[i]; i++; }
    dest[i] = '\0';
}

int my_strcmp(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return a[i] - b[i];
        i++;
    }
    return a[i] - b[i];
}

/*
 * my_tokenize — splits input by delim in-place.
 * Writes pointers into tokens[]. Returns token count.
 * WARNING: modifies input string.
 */
int my_tokenize(char *input, char *tokens[], int max, char delim) {
    int count = 0;
    int i = 0;
    int len = my_strlen(input);

    while (i < len && input[i] == delim) i++;   /* skip leading */

    while (i < len && count < max) {
        tokens[count++] = &input[i];
        while (i < len && input[i] != delim) i++;
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
