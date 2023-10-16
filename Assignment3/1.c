#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main() {
    // printf("Hello\n");
    int n_values[] = {45, 10};
    int num_values = sizeof(n_values) / sizeof(n_values[0]);

    for (int i = 0; i < num_values; i++) {
        int n = n_values[i];
        pid_t child_pid;

        // Fork a child process
        child_pid = fork();

        if (child_pid == 0) {
            // This is the child process
            // Calculate Fibonacci
            int result = fibonacci(n);
            printf("(%d)th Fibonacci number = %d\n", n, result);
            exit(0);
        } else if (child_pid > 0) {
            // This is the parent process
            sleep(5); // Sleep for 5 seconds

            // Send a signal to the child process to stop it
            kill(child_pid, SIGSTOP);
            printf("Fibonacci calculation for %d paused\n", n);

            // Wait for the child process to finish
            int status;
            wait(&status);

            // Send a signal to the child process to resume it
            kill(child_pid, SIGCONT);
            printf("Fibonacci calculation for %d resumed\n", n);
        } else {
            perror("Fork failed");
            exit(1);
        }
    }

    return 0;
}
