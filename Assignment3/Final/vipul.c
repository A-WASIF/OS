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
#include <time.h>

typedef struct 
{
    char *command;
    double execution_time;

}HistoryEntry;




void addToHistory(HistoryEntry *history, char *command, double execution_time, int index)
{
    history[index].command = strdup(command);
    history[index].execution_time = execution_time;
}

void printHistory(HistoryEntry *history, int historyIndex)
{
    printf("\nCommand History:\n");
    printf("*****************************************\n");
    for (int i = 0; i < historyIndex; i++)
    {
        printf("%s - Execution Time: %.9f seconds\n", history[i].command, (history[i].execution_time)/1e6);
        free(history[i].command); 
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

typedef struct QNode
{
    pid_t pid;
    char *command;
    int state;
    struct timespec start_time;
    struct timespec end_time;
    struct QNode *next;
} QNode;

struct Queue {
	QNode *front, *rear;
};

QNode* newNode(int k, char* command)
{
	QNode* temp = (QNode*) malloc (sizeof(QNode));
	temp->pid = k;
    temp->state = 0;
    temp->command = command;
	temp->next = NULL;
	return temp;
}

struct Queue* createQueue()
{
	struct Queue* q  = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;
	return q;
}

void enQueue(struct Queue* q, int k, char* command)
{

	QNode* temp = newNode(k, command);

	if (q->rear == NULL) {
		q->front = q->rear = temp;
		return;
	}

	q->rear->next = temp;
	q->rear = temp;
}

QNode* deQueue(struct Queue* q)
{
    pid_t id = q->front->pid;


	QNode* temp = q->front;

	q->front = q->front->next;

	if (q->front == NULL)
		q->rear = NULL;


    return temp;
}

bool isEmpty(struct Queue* q)
{
    return (q->front == NULL);
}


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
    // struct timespec start_time, end_time;

   
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
                // struct timespec start_time;
                clock_gettime(CLOCK_MONOTONIC, &temp->start_time);
                 //gettimeofday(&temp->start_time, NULL);

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
                if(state == 0){
                    enQueue(q, temp->pid, temp->command);
                }
                else
                {
                    // struct timespec end_time;
                    clock_gettime(CLOCK_MONOTONIC, &temp->end_time);

                    double execution_time = (temp->end_time.tv_sec - temp->start_time.tv_sec) +
                                            (temp->end_time.tv_nsec - temp->start_time.tv_nsec) / 1000000.0;

                    // Store execution time in the history entry
                    addToHistory(history, temp->command, execution_time, historyIndex);
                    ++historyIndex;
                    --numProcess;
                }    
                
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