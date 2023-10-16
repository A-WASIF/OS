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
    // char * command;
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
QNode* newNode(int k)
{
	QNode* temp = (QNode*) malloc (sizeof(QNode));
	temp->pid = k;
    temp->state = 0;
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

// The function to add a pid k to q
void enQueue(struct Queue* q, int k)
{
	// Create a new LL node
	QNode* temp = newNode(k);

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

// Function to remove a pid from given queue q
QNode* deQueue(struct Queue* q)
{
	// // If queue is empty, return NULL.
	// if (q->front == NULL)
	// 	return;

    pid_t id = q->front->pid;

	// Store previous front and move front one node ahead
	QNode* temp = q->front;

	q->front = q->front->next;

	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
		q->rear = NULL;

	// free(temp);

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


int main() {

    // Register the signal handler for Ctrl+C
    signal(SIGINT, signal_handler);

    // Create a shared memory segment
    // key_t key = ftok("shmfile", 65);
    // int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);

    // if (shmid == -1) {
    //     perror("shmget");
    //     exit(1);
    // }
    
    int ncpu, tslice;
    char *input;

    printf("Enter Number of CPU's to be used: ");
    scanf("%d", &ncpu);

    printf("Enter Time Slice: ");
    scanf("%d", &tslice);
    getchar();

    // // Attach shared memory to the process
    // struct Queue* q = (struct Queue*)shmat(shmid, NULL, 0);
    
    struct Queue* q =createQueue();

    int arr[] = {45, 10, 3, 2};
    int i = 0;
    volatile int array[4] = {0};


    while(true){
        input = userinput();
        if(strcmp(input, "end") == 0) break;
        
        else if(strncmp(input, "submit ", 7) != 0){
            printf("Wrong Input Method!\n");
        }

        else{
            pid_t create_process = fork();

            if(create_process == 0){
                // QNode* current = q->front;
                int index = i;
                int value = fibonacci(arr[i]);
                printf("Fibonacci value of %d is: %d\n", arr[i], value);
                array[index] = 1; 
                printf("status of %d : %d\n", index, array[index]);
                
                // while (current->pid != create_process) {
                //     current = current->next;
                // }
                // printf("Process ends\n");
                // current->state = 1;
                // printf("State of end sequence with pid %d : %d\n", current->pid, current->state);
                
                // free(current);
                
                exit(0);
            }

            else if(create_process > 0){
                kill(create_process, SIGSTOP);
                enQueue(q, create_process);
            }

            else{
                perror("Error in forking");
            }
            printf("%s\n", input + 7);
            ++i;
        }
    }

    printQueue(q);

    printf("%d\n", isEmpty(q));

    while(!isEmpty(q)){
        for(int i = 0; i < ncpu; i++){
            QNode* temp = deQueue(q);
            // pid_t process_id = deQueue(q)->pid;
        
            pid_t child_pid = fork();

            if (child_pid == 0) {
                printf("Child process %u has started\n", temp->pid);
                kill(temp->pid, SIGCONT);
                exit(0);
            }
            
            else if (child_pid > 0) {
                // int ind = 0;

                // QNode* current = q->front;
                // while (current->pid != temp->pid) {
                //     ++ind;
                // }
                // printf("Process ends\n");
                // current->state = 1;
                // printf("State of end sequence with pid %d : %d\n", current->pid, current->state);
                
                // free(current);                

                sleep(tslice);

                // Stop the child process
                kill(temp->pid, SIGSTOP);
                // printf("Child process paused\n");
                // for(int j = 0; j < 4; i++){
                //     printf("%d ", array[j]);
                // }

                // printf("\n");

                // if(array[ind] != 1)
                enQueue(q, temp->pid);
            }
            
            else {
                perror("Fork failed");
                // exit(1);
            }

        }
    }

    // shmdt(q);





    // struct Queue* q = createQueue();
    // enQueue(q, 10);
    // enQueue(q, 20);
    // deQueue(q);
    // deQueue(q);
    // enQueue(q, 30);
    // enQueue(q, 40);
    // enQueue(q, 50);
    // deQueue(q);

    // Print all elements in the queue
    

    return 0;
}

void signal_handler(int signum) {
    printf("\nCtrl+C detected. Exiting.\n");
    exit(0);
}