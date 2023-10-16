#include <stdio.h>
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main() {
    int n = 12;

    // printf("Enter the value of n: ");
    // scanf("%d", &n);

    if (n < 0) {
        printf("Please enter a non-negative integer.\n");
    } else {
        printf("(%d)th Fibonacci number = %d\n", n, fibonacci(n));
    }
    return 0;
}
