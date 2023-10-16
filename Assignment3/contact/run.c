#include <stdio.h>
#include "fib.h"

int main() {
    int n = 40;

    if (n < 0) {
        printf("Please enter a non-negative integer.\n");
    } else {
        printf("(%d)th Fibonacci number = %d\n", n, fibonacci(n));
    }
    return 0;
}
