  
                                        
                                                                 
   
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
