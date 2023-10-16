#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

struct Queue {
    int* values;
    int front, rear, size, capacity;
};

struct Queue* createQueue(int capacity) {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = capacity - 1;
    queue->values = (int*)malloc(sizeof(int) * capacity);
    return queue;
}

int isFull(struct Queue* queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}

void enqueue(struct Queue* queue, int item) {
    if (isFull(queue)) {
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->values[queue->rear] = item;
    queue->size++;
}

int dequeue(struct Queue* queue) {
    if (isEmpty(queue)) {
        return -1; // Queue is empty
    }
    int item = queue->values[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}

volatile int timeout_flag = 0;

void timeout_handler(int signum) {
    timeout_flag = 1;
}

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

int main() {
    int n_values[] = {45, 40};
    int num_values = sizeof(n_values) / sizeof(n_values[0]);

    struct Queue* queue = createQueue(num_values);

    for (int i = 0; i < num_values; i++) {
        enqueue(queue, n_values[i]);
    }

    while (!isEmpty(queue)) {
        int n = dequeue(queue);
        pid_t child_pid;

        // Set up a timeout handler
        signal(SIGALRM, timeout_handler);
        timeout_flag = 0;

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
            // Sleep for 5 seconds
            sleep(5);

            // Check if the timeout occurred
            if (timeout_flag) {
                printf("Fibonacci calculation for %d timed out\n", n);
                kill(child_pid, SIGKILL); // Terminate the child process
            } else {
                // Wait for the child process to finish
                int status;
                wait(&status);
            }
        } else {
            perror("Fork failed");
            exit(1);
        }
    }

    return 0;
}
