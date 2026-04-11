#include "string.h"

/* Return length of string */
int my_strlen(const char *s) {
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

/* Copy src into dest */
void my_strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') { dest[i] = src[i]; i++; }
    dest[i] = '\0';
}

/* Compare two strings: 0 = equal, non-zero = different */
int my_strcmp(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return a[i] - b[i];
        i++;
    }
    return a[i] - b[i];
}

/* Concatenate src onto end of dest */
void my_strcat(char *dest, const char *src) {
    int i = my_strlen(dest);
    int j = 0;
    while (src[j] != '\0') { dest[i++] = src[j++]; }
    dest[i] = '\0';
}

/* Split input string by delim into tokens array. Returns token count.
   WARNING: modifies input string (replaces delim with '\0') */
int my_tokenize(char *input, char *tokens[], int max_tokens, char delim) {
    int count = 0;
    int i = 0;
    int len = my_strlen(input);

    /* Skip leading delimiters */
    while (i < len && input[i] == delim) i++;

    while (i < len && count < max_tokens) {
        tokens[count++] = &input[i];
        while (i < len && input[i] != delim) i++;
        if (i < len) { input[i] = '\0'; i++; }
        while (i < len && input[i] == delim) i++;
    }
    return count;
}

/* Convert integer to string */
void my_int_to_str(int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    int i = 0;
    int neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    char tmp[20];
    while (n > 0) { tmp[i++] = '0' + (n % 10); n /= 10; }
    if (neg) tmp[i++] = '-';
    /* Reverse */
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

/* Convert string to integer */
int my_str_to_int(const char *s) {
    int result = 0;
    int sign = 1;
    int i = 0;
    if (s[i] == '-') { sign = -1; i++; }
    while (s[i] >= '0' && s[i] <= '9') {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    return sign * result;
}

/* Convert string to uppercase in-place */
void my_str_to_upper(char *s) {
    for (int i = 0; s[i]; i++) {
        if (s[i] >= 'a' && s[i] <= 'z') s[i] -= 32;
    }
}

/* Check if string starts with prefix: 1 = yes, 0 = no */
int my_str_starts_with(const char *s, const char *prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (s[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}
