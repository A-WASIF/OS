#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

#define NCPU 2         // Number of CPU resources
#define TSLICE 1000    // Time quantum in milliseconds
#define MAX_PROCESSES 100

#define HISTORY_FILE "history.txt"
FILE* file_History;

// Data structure to store command history with process IDs and execution times
struct CommandHistory {
    char command[256];
    pid_t pid;
    double execution_time;
};

// Define a structure for a process node in the linked list
typedef struct ProcessNode {
    pid_t process;
    // int state;
    // int priority;
    struct ProcessNode* next;
} ProcessNode;

ProcessNode* front = NULL;  // Front of the linked list
ProcessNode* rear = NULL;   // Rear of the linked list
volatile int schedulerRunning = 0;

// Function to add a process to the ready queue (linked list)
void addToReadyQueue(pid_t process) {
    ProcessNode* newNode = (ProcessNode*)malloc(sizeof(ProcessNode));
    if (newNode == NULL) {
        perror("malloc");
        exit(1); // Handle memory allocation error
    }
    newNode->process = process;
    newNode->next = NULL;

    if (rear == NULL) {
        // The queue is empty
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

// Function to remove a process from the ready queue (linked list)
pid_t removeFromReadyQueue() {
    if (front == NULL) {
        return -1; // Queue is empty
    }

    pid_t process = front->process;
    ProcessNode* temp = front;

    front = front->next;
    if (front == NULL) {
        rear = NULL; // The queue is now empty
    }

    free(temp);
    return process;
}

// Signal handler for the scheduler
void schedulerHandler(int signum);

// Checking inputs and calling for Execution
int launch(char *input_command, struct CommandHistory *history, int *history_count);

void run_command(char *input_command, struct CommandHistory *history, int *history_count);
    


// Show history of commands written so far in program during execution program
void tempHistory();

int main() {
    // Register signal handlers
    signal(SIGUSR1, schedulerHandler);
    signal(SIGALRM, schedulerHandler);

    // Start the scheduler loop
    while (1) {
        // Wait for a signal from SimpleShell to start scheduling
        while (!schedulerRunning) {
            usleep(100000); // Sleep for 100 milliseconds
        }

        // Schedule processes
        for (int i = 0; i < NCPU; i++) {
            pid_t process = removeFromReadyQueue();
            if (process > 0) {
                pid_t child_pid = fork();

                if (child_pid < 0) {
                    perror("Error in Forking!");
                }

                else if (child_pid == 0) {
                    // Child process
                    printf("Scheduled process %d\n", process);
                    // Execute the process or task here
                    // ...
                    exit(0); // Exit the child process
                }
            }
        }
        // Wait for the time quantum to expire
        usleep(TSLICE * 1000); // Convert TSLICE to microseconds

        // Stop scheduled processes and add them back to the ready queue (linked list)
        for (int i = 0; i < NCPU; i++) {
            pid_t process = removeFromReadyQueue();
            if (process > 0) {
                kill(process, SIGSTOP);
                addToReadyQueue(process);
                printf("Stopped process %d\n", process);
            }
        }

        // Signal SimpleShell that scheduling is complete
        kill(getppid(), SIGUSR2);
    }

    return 0;
}


// Signal handler for the scheduler
void schedulerHandler(int signum) {
    if (signum == SIGUSR1) {
        schedulerRunning = 1;
    } else if (signum == SIGALRM) {
        schedulerRunning = 0;
    }
}

// Checking inputs and calling for Execution
int launch(char *input_command, struct CommandHistory *history, int *history_count) {
    // Add the command to history
    // for tracing the history
    file_History = fopen(HISTORY_FILE, "a");
    if (file_History == NULL){
        perror("Error opening file");
    }
    
    else{
        fprintf(file_History, "%s\n", input_command);
        fclose(file_History);
    }

    if (strcmp(input_command, "history") == 0) {
        tempHistory();
    }
    
    else {
        run_command(input_command, history, history_count);
    }

    return 1;
}

void run_command(char *input_command, struct CommandHistory *history, int *history_count){
    pid_t pid;
    int status;
    struct timeval start_time, end_time;

    // Record start time before forking
    gettimeofday(&start_time, NULL);

    // Create a child process
    pid = fork();

    if(pid < 0){
        perror("Fork Failed");
    }
    
    else if(pid == 0){
        // In the child process execute the command
        execl("/bin/sh", "sh", "-c", input_command, (char *)NULL);

        perror("Execution command error");
        exit(EXIT_FAILURE);
    }

    else{
        // In the parent process wait for the child
        waitpid(pid, &status, 0);

        // Record end time
        gettimeofday(&end_time, NULL);

        if (WIFEXITED(status)){
            double execution_time = (double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_usec - start_time.tv_usec) / 1e6;

            // Storing command, pid, and execution time in the history
            snprintf(history[*history_count].command, sizeof(history[*history_count].command), "%s", input_command);
            history[*history_count].pid = pid;
            history[*history_count].execution_time = execution_time;
            (*history_count)++;
        }
    }

    return;
}

// Show history of commands written so far in program during execution program
void tempHistory(){
        char chr;
        FILE * fd;

        fd = fopen("history.txt", "r");

        if(fd == NULL){
            perror("Error in Opening History!");
        }

        while((chr = fgetc(fd)) != EOF) printf("%c", chr);

        fclose(fd);

        return;
}