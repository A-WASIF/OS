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
    printf("Hello\n");
    int arr[] = {45, 42, 43, 44};
    
    for(int i = 0; i < 4; i++){
        int n = arr[i];
        pid_t child_pid;

        child_pid = fork();

        if (child_pid == 0) {
            int result = fibonacci(n);
            printf("(%d)th Fibonacci number = %d\n", n, result);
            exit(0);
        }
        
        else if (child_pid > 0) {
            sleep(2);

            // Stop the child process
            kill(child_pid, SIGSTOP);
            printf("Child process paused\n");

            sleep(2);

            // Resume the child process
            kill(child_pid, SIGCONT);
            printf("Child process resumed\n");
    
            // int status;
            // wait(&status);
        }
        
        else {
            perror("Fork failed");
            exit(1);
        }
    }

    return 0;
}
