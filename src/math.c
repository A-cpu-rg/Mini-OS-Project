  
                                        
                                                                 
   
#include "math.h"

                                      
int my_multiply(int a, int b) {
    int result = 0;
    int negative = 0;
    if (b < 0) { b = -b; negative = !negative; }
    if (a < 0) { a = -a; negative = !negative; }
    for (int i = 0; i < b; i++) result += a;
    return negative ? -result : result;
}

                                                 
int my_divide(int a, int b) {
    if (b == 0) return 0;
    int negative = 0;
    if (a < 0) { a = -a; negative = !negative; }
    if (b < 0) { b = -b; negative = !negative; }
    int result = 0;
    while (a >= b) { a -= b; result++; }
    return negative ? -result : result;
}

int my_abs(int n) {
    return (n < 0) ? -n : n;
}

int my_modulo(int a, int b) {
    if (b == 0) return 0;

    int dividend = my_abs(a);
    int divisor = my_abs(b);
    while (dividend >= divisor) dividend -= divisor;

    return (a < 0) ? -dividend : dividend;
}

int my_in_bounds(int value, int min, int max) {
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    return (value >= min && value <= max) ? 1 : 0;
}

int my_clamp(int value, int min, int max) {
    if (min > max) {
        int tmp = min;
        min = max;
        max = tmp;
    }
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
