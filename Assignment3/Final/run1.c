#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdbool.h>
// #include "FIBONACCI.h"
// #define SHM_SIZE 1024


// A linked list (LL) node to store a queue entry
typedef struct QNode {
	pid_t pid;
    char * command;
    int state;
    // int priority;
	struct QNode* next;
} QNode;

// The queue, front stores the front node of LL and rear
// stores the last node of LL
struct Queue {
	QNode *front, *rear;
};

// A utility function to create a new linked list node.
QNode* newNode(int k, char* command)
{
	QNode* temp = (QNode*) malloc (sizeof(QNode));
	temp->pid = k;
    temp->state = 0;
    temp->command = command;
	temp->next = NULL;
	return temp;
}

// A utility function to create an empty queue
struct Queue* createQueue()
{
	struct Queue* q  = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;
	return q;
}

void enQueue(struct Queue* q, int k, char* command)
{
	// Create a new LL node
	QNode* temp = newNode(k, command);

	// If queue is empty, then new node is front and rear
	// both
	if (q->rear == NULL) {
		q->front = q->rear = temp;
		return;
	}

	// Add the new node at the end of queue and change rear
	q->rear->next = temp;
	q->rear = temp;
}

QNode* deQueue(struct Queue* q)
{
    pid_t id = q->front->pid;

	// Store previous front and move front one node ahead
	QNode* temp = q->front;

	q->front = q->front->next;

	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
		q->rear = NULL;


    return temp;
}

bool isEmpty(struct Queue* q)
{
    return (q->front == NULL);
}

// Function to print all elements in the queue
void printQueue(struct Queue* q) {
    QNode* current = q->front;
    printf("Queue Elements: ");
    while (current != NULL) {
        printf("%d ", current->pid);
        current = current->next;
    }
    printf("\n");
}

void signal_handler(int signum);

// Take input from Users
char *userinput(){
    int size = 100;
    char *command = malloc(sizeof(char) * size);
    if (command == NULL) {
        printf("%s\n", "Could not allocate memory!! Try again");
        command[0] = '\0';
        return command;
    }

    else{
        printf("SimpleShell$ ");        
        fgets(command, size, stdin);
        command[strcspn(command, "\n")] = '\0';
    }
    
    return command;
}

// Fibonacci Program
int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}

void launch(char *exact_command){
    execlp(exact_command, exact_command, NULL);
}

void updateProcessState(struct Queue* q, pid_t pid, int state) {
    QNode* current = q->front;

    while (current != NULL) {
        if (current->pid == pid) {
            current->state = state;
            break;
        }
        current = current->next;
    }
}

int main() {
    printf("starts\n");

    // Register the signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    int ncpu, tslice;
    char *input;

    printf("Enter Number of CPU's to be used: ");
    scanf("%d", &ncpu);

    printf("Enter Time Slice: ");
    scanf("%d", &tslice);
    getchar();

   
    struct Queue* q = createQueue();

    while(true){
        input = userinput();
        if(strcmp(input, "end") == 0) break;
        
        else if(strncmp(input, "submit ", 7) != 0){
            printf("Wrong Input Method!\n");
        }

        else{
            char* exact_command = input + 7;
            pid_t create_process = fork();
            
            if(create_process == 0){                
                exit(0);
            }

            else if(create_process > 0){
                kill(create_process, SIGSTOP);
                enQueue(q, create_process, exact_command);
            }

            else{
                perror("Error in forking");
            }
        }
    }

    printQueue(q);

    printf("%d\n", isEmpty(q));

    while (!isEmpty(q)) {
        for (int i = 0; i < ncpu; i++) {
            if (isEmpty(q)) {
                break;  // No more processes in the queue
            }
            QNode* temp = deQueue(q);

            pid_t child_pid = fork();

            if (child_pid == 0) {
                printf("Child process %u has started\n", temp->pid);
                kill(temp->pid, SIGCONT);
                execlp(temp->command, temp->command, NULL);  // Execute the given executable
                // If execlp fails, handle the error here
                exit(1);
            } else if (child_pid > 0) {
                // Wait for the child process to finish and update its state
                sleep(tslice);
                int status;
                waitpid(child_pid, &status, 0);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    // The process exited successfully
                    updateProcessState(q, temp->pid, 1);
                }
            } else {
                perror("Fork failed");
                // Handle the error as needed
            }
        }
    }
    return 0;
}

void signal_handler(int signum) {
    printf("\nCtrl+C detected. Exiting.\n");
    exit(0);
}