#include "math.h"

/* Multiply two integers using repeated addition */
int my_multiply(int a, int b) {
    int result = 0;
    int negative = 0;
    if (b < 0) { b = -b; negative = !negative; }
    if (a < 0) { a = -a; negative = !negative; }
    for (int i = 0; i < b; i++) result += a;
    return negative ? -result : result;
}

/* Integer division */
int my_divide(int a, int b) {
    if (b == 0) return 0; /* Guard against divide by zero */
    int negative = 0;
    if (a < 0) { a = -a; negative = !negative; }
    if (b < 0) { b = -b; negative = !negative; }
    int result = 0;
    while (a >= b) { a -= b; result++; }
    return negative ? -result : result;
}

/* Modulo operation */
int my_modulo(int a, int b) {
    if (b == 0) return 0;
    int div = my_divide(a, b);
    return a - my_multiply(div, b);
}

/* Absolute value */
int my_abs(int a) {
    return a < 0 ? -a : a;
}

/* Return larger of two values */
int my_max(int a, int b) {
    return a > b ? a : b;
}

/* Return smaller of two values */
int my_min(int a, int b) {
    return a < b ? a : b;
}

/* Clamp value within [min, max] range */
int my_clamp(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}
