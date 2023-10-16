#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// Define a signal handler function to handle Ctrl+C
void signal_handler(int signum) {
    printf("\nCtrl+C detected. Exiting.\n");
    exit(0);
}

int main() {
    // Register the signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    // Infinite loop
    while (1) {
        // Your code here (this loop will continue until Ctrl+C is pressed)
        printf("Running...\n");
        sleep(1);
    }

    return 0;
}
