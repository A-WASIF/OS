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

typedef struct {
    char* command;
} HistoryEntry;

void addToHistory(HistoryEntry* history, char* command, int index) {
    history[index].command = strdup(command);
}

void printHistory(HistoryEntry* history, int historyIndex) {
    printf("\nCommand History:\n");
    printf("*****************************************\n");
    for (int i = 0; i < historyIndex; i++) {
        printf("%d: %s\n", i + 1, history[i].command);
        free(history[i].command);  // Free the memory allocated for each command
    }
    printf("*****************************************\n");
}
void printStatus(int status, pid_t pid, char* command) {
    if (WIFEXITED(status)) {
        printf("Process %d (%s) exited with status %d\n", pid, command, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Process %d (%s) terminated by signal %d\n", pid, command, WTERMSIG(status));
    }
}
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
    printf("Queue Elements (PID, Command): ");
    while (current != NULL) {
        printf("(%d, %s) ", current->pid, current->command);
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

void launch(char *exact_command){
    execlp(exact_command, exact_command, NULL);
}

int min(int a, int b){
    return a < b ? a : b;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NCPU> <TSLICE>\n", argv[0]);
        return 1;
    }
    int NCPU = atoi(argv[1]);
    int TSLICE = atoi(argv[2]);
    
    signal(SIGINT, signal_handler);
    char *input;

   
    struct Queue* q = createQueue();
    int numProcess = 0;
    int historySize = 100;
    HistoryEntry history[historySize];
    int historyIndex = 0;
    

    while(true){
        input = userinput();
        if(strcmp(input, "end") == 0) break;
        
        else if(strncmp(input, "submit ", 7) != 0){
            printf("Wrong Input Method!\n");
        }

        else{
            char* exact_command = input + 7;

            addToHistory(history, exact_command, historyIndex);
            historyIndex++;
            
            pid_t create_process = fork();
            
            if(create_process == 0){
                launch(exact_command);
                exit(0);
            }

            else if(create_process > 0){
                kill(create_process, SIGSTOP);
                enQueue(q, create_process, exact_command);
                ++numProcess;
            }

            else{
                perror("Error in forking");
            }
        }
    }

    printf("Number of process : %d\n", numProcess);
    printQueue(q);
   
    while(!isEmpty(q)){
        int process = min(numProcess, NCPU);

        for(int i = 0; i < process; i++){
            QNode* temp = deQueue(q);
        
            pid_t child_pid = fork();

            if (child_pid == 0) {
                printf("Child process %u has started\n", temp->pid);
                kill(temp->pid, SIGCONT);
                exit(0);
            }
            
            else if (child_pid > 0) {

                sleep(TSLICE);

                // Stop the child process
                kill(temp->pid, SIGSTOP);

                int num = 1;
                int state = waitpid(temp->pid, &num, WNOHANG);
                if(state == 0) enQueue(q, temp->pid, temp->command);
                else --numProcess;
            }
            
            else {
                perror("Fork failed");
            }
        }
        
        if(!isEmpty(q)){
            printf("\n");
            printQueue(q);
            printf("\n");
        }
    }

    printHistory(history, historyIndex);
    return 0;
}

void signal_handler(int signum) {
    printf("\nCtrl+C detected. Exiting.\n");
    exit(0);
}
