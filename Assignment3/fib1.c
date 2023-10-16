#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main() {
    int n = 45;
    pid_t child_pid;

    // Fork a child process
    child_pid = fork();

    if (child_pid == 0) {
        // This is the child process
        int result = fibonacci(n);
        printf("(%d)th Fibonacci number = %d\n", n, result);
    } else if (child_pid > 0) {
        // This is the parent process
        sleep(7);
        printf("Hello\n");

        // Wait for the child process to finish
        int status;
        wait(&status);
    } else {
        perror("Fork failed");
        exit(1);
    }

    return 0;
}
