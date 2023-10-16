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
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

typedef struct {
    char* command;
    pid_t pid;
    int priority;
    double execution_time;
    double wait_time;
} HistoryEntry;

void addToHistory(HistoryEntry* history, char* command, pid_t pid, int priority, int index) {
    history[index].command = strdup(command);
    history[index].pid = pid;
    history[index].priority = priority;
}

void printHistory(HistoryEntry* history, int historyIndex) {
    printf("\nCommand History(Command, Pid, Execution Time, Waiting Time):\n");
    printf("*****************************************\n");
    for (int i = 0; i < historyIndex; i++) {
        printf("%d: %s      %u      %.3f millisec.     %.3f millisec. \n", i + 1, history[i].command, history[i].pid, history[i].execution_time, history[i].wait_time);
        
        free(history[i].command);
    }
    printf("*****************************************\n");
}

// A linked list (LL) node to store a queue entry
typedef struct QNode {
	pid_t pid;
    char * command;
    int state;
    int priority;
    double execution_time;
	struct QNode* next;
} QNode;

// The queue, front stores the front node of LL and rear
// stores the last node of LL
struct Queue {
	QNode *front, *rear;
};

// A utility function to create a new linked list node.
QNode* newNode(int k, char* command, int priority, double exe_time)
{
	QNode* temp = (QNode*) malloc (sizeof(QNode));
	temp->pid = k;
    temp->state = 0;
    temp->priority = priority;
    temp->command = command;
    temp->execution_time = exe_time;
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

void enQueue(struct Queue* q, int k, char* command, int priority, double exe_time)
{
	// Create a new LL node
	QNode* temp = newNode(k, command, priority, exe_time);

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
    printf("Elements of Queue %d (PID, Command, Priority): ", current->priority);
    while (current != NULL) {
        printf("(%d, %s) ", current->pid, current->command);
        current = current->next;
    }
    printf("\n");
}


typedef struct {
    sem_t mutex;
    struct Queue q1;
    struct Queue q2;
    struct Queue q3;
    struct Queue q4;
} shm_t;


int checkforpriority(char* input){
    int length = strlen(input);

    if(input[length - 2] == ' ' && isdigit(input[length - 1])) return 1;
    return 0;
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
    int check = execlp(exact_command, exact_command, NULL);

    if(check == -1) {
        printf("No Such File Found or File is Not Executable: %s\n", exact_command);
        perror("execlp");
    }
}

void printAllQueue(struct Queue* q1, struct Queue* q2, struct Queue* q3, struct Queue* q4);

int min(int a, int b){
    return a < b ? a : b;
}

void scheduler(int* numProcess, int NCPU, double TSLICE, struct Queue* q, struct Queue* Q, int *Qnum, HistoryEntry history[], int historyIndex);

void initializeWaitTime(HistoryEntry history[], int historyIndex, int priority, double TSLICE, int prev_numProcess){
    for(int i = 0; i < historyIndex; i++){
        if(history[i].priority == priority) history[i].wait_time = prev_numProcess * TSLICE;
    }
}

void calWaitTime(struct Queue* q, double TSLICE, int NCPU, HistoryEntry history[], int historyIndex){
    int ind = 0;
    QNode* current = q->front;
    while (current != NULL) {
        for(int i = 0; i < historyIndex; i++){
            if(current->pid == history[i].pid) history[i].wait_time += (ind / NCPU) * TSLICE;
        }
        current = current->next;
        ++ind;
    }
}

int numProcess1 = 0;
int numProcess2 = 0;
int numProcess3 = 0;
int numProcess4 = 0;

void prepAndLaunch_schduler(struct Queue* q1, struct Queue* q2, struct Queue* q3, struct Queue* q4, double TSLICE, int NCPU, HistoryEntry history[], int historyIndex){
    printf("Number of process in q1: %d\n", numProcess1);
    printf("Number of process in q2: %d\n", numProcess2);
    printf("Number of process in q3: %d\n", numProcess3);
    printf("Number of process in q4: %d\n", numProcess4);

    initializeWaitTime(history, historyIndex, 1, TSLICE, 0);
    initializeWaitTime(history, historyIndex, 2, TSLICE, numProcess1 / NCPU);
    initializeWaitTime(history, historyIndex, 3, TSLICE, (numProcess2 + numProcess1) / NCPU);
    initializeWaitTime(history, historyIndex, 4, TSLICE, (numProcess3 + numProcess2 + numProcess1) / NCPU);

    printAllQueue(q1, q2, q3, q4);
    calWaitTime(q1, TSLICE, NCPU, history, historyIndex);
    scheduler(&numProcess1, NCPU, TSLICE, q1, q2, &numProcess2, history, historyIndex);

    printAllQueue(q1, q2, q3, q4);
    calWaitTime(q2, TSLICE, NCPU, history, historyIndex);
    scheduler(&numProcess2, NCPU, TSLICE, q2, q3, &numProcess3, history, historyIndex);

    printAllQueue(q1, q2, q3, q4);
    calWaitTime(q3, TSLICE, NCPU, history, historyIndex);
    scheduler(&numProcess3, NCPU, TSLICE, q3, q4, &numProcess4, history, historyIndex);

    printAllQueue(q1, q2, q3, q4);
    calWaitTime(q4, TSLICE, NCPU, history, historyIndex);
    scheduler(&numProcess4, NCPU, TSLICE, q4, q1, &numProcess1, history, historyIndex);

}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <NCPU> <TSLICE>\n", argv[0]);
        return 1;
    }
    int NCPU = atoi(argv[1]);
    double TSLICE = atoi(argv[2]);
    
    signal(SIGINT, signal_handler);
    
    int shm_fd;
    shm_t* shm;

    // Create a shared memory segment
    shm_fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    // Set the size of the shared memory segment
    ftruncate(shm_fd, sizeof(shm_t));

    // Map the shared memory segment into your process's memory space
    shm = (shm_t*) mmap (0, sizeof(shm_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // Initialize the semaphore
    sem_init(&shm->mutex, 1, 0);

    if (&shm->mutex == SEM_FAILED) {
        perror("sem_open failed");
        return 1;
    }

    char *input;
   
    struct Queue* q1 = createQueue();
    struct Queue* q2 = createQueue();
    struct Queue* q3 = createQueue();
    struct Queue* q4 = createQueue();

    // Place Queues into shared memory
    memcpy(&shm->q1, q1, sizeof(struct Queue));
    memcpy(&shm->q2, q2, sizeof(struct Queue));
    memcpy(&shm->q3, q3, sizeof(struct Queue));
    memcpy(&shm->q4, q4, sizeof(struct Queue));

    int historySize = 100;
    HistoryEntry history[historySize];
    int historyIndex = 0;

    while(true){
        input = userinput();
        
        if(strcmp(input, "end") == 0) break;

        // else if(strcmp(input, "run") == 0){
        //     sem_wait(&shm->mutex);

        //     while(!isEmpty(q1) || !isEmpty(q2) || !isEmpty(q3) || !isEmpty(q4)) prepAndLaunch_schduler(q1, q2, q3, q4, TSLICE, NCPU, history, historyIndex);

        //     sem_post(&shm->mutex);
        // }
        
        else if(strncmp(input, "submit ", 7) != 0){
            printf("Wrong Input Method!\n");
        }

        else{
            int check = checkforpriority(input);
            
            char* exact_command;
            if(!check)  exact_command = input + 7;

            else{
                int start_position = 7;
                int end_position = strlen(input) - 2;
                int length = end_position - start_position;

                exact_command = (char *)malloc(length + 1); // Allocate memory for exact_command
                strncpy(exact_command, input + start_position, length);
                exact_command[length] = '\0';
            }

            pid_t create_process = fork();

            if(create_process == 0){
                launch(exact_command);

                munmap(shm, sizeof(shm_t));
                close(shm_fd);
                exit(0);
            }

            else if(create_process > 0){
                kill(create_process, SIGSTOP);

                sem_wait(&shm->mutex);

                while(!isEmpty(q1) || !isEmpty(q2) || !isEmpty(q3) || !isEmpty(q4)) prepAndLaunch_schduler(q1, q2, q3, q4, TSLICE, NCPU, history, historyIndex);

                sem_post(&shm->mutex);
                
                int priority = 1;
                if(check){
                    priority = input[strlen(input) - 1] - '0';
                }

                addToHistory(history, exact_command, create_process, priority, historyIndex);
                historyIndex++;
                
                if(priority == 1){
                    enQueue(q1, create_process, exact_command, priority, 0);
                    ++numProcess1;
                }

                else if(priority == 2){
                    enQueue(q2, create_process, exact_command, priority, 0);
                    ++numProcess2;
                }

                else if(priority == 3){
                    enQueue(q3, create_process, exact_command, priority, 0);
                    ++numProcess3;
                }

                else{
                    enQueue(q4, create_process, exact_command, priority, 0);
                    ++numProcess4;
                }
            }

            else{
                perror("Error in forking");
            }
        }
    }

    printHistory(history, historyIndex);

    // Detach and clean up shared memory
    munmap(shm, sizeof(shm_t));
    close(shm_fd);
    shm_unlink("/my_shm");    
    sem_destroy(&shm->mutex);

    return 0;
}

void signal_handler(int signum) {
    printf("\nCtrl+C detected.\n");
    exit(0);
}

void printAllQueue(struct Queue* q1, struct Queue* q2, struct Queue* q3, struct Queue* q4){
    printf("\n");
    printf("*****************************************\n");
    if(!isEmpty(q1)){
        printQueue(q1);
        printf("\n");
    }

    if(!isEmpty(q2)){
        printQueue(q2);
        printf("\n");
    }

    if(!isEmpty(q3)){
        printQueue(q3);
        printf("\n");
    }
    
    if(!isEmpty(q4)) printQueue(q4);
    printf("*****************************************\n");
    printf("\n");
}

void scheduler(int* numProcess, int NCPU, double TSLICE, struct Queue* q, struct Queue* Q, int *Qnum, HistoryEntry history[], int historyIndex){
    while(!isEmpty(q)){
        int process = min((*numProcess), NCPU);

        for(int i = 0; i < process; i++){
            QNode* temp = deQueue(q);
        
            pid_t child_pid = fork();

            if (child_pid == 0) {
                printf("Child process %u has started\n", temp->pid);
                kill(temp->pid, SIGCONT);
                exit(0);
            }
            
            else if (child_pid > 0) {

                sleep(TSLICE / 1000);

                // Stop the child process
                kill(temp->pid, SIGSTOP);

                temp->execution_time += TSLICE;
                
                int num = 1;
                int state = waitpid(temp->pid, &num, WNOHANG);
                if(state == 0){
                    if(temp->priority != 4){
                        ++temp->priority;
                        enQueue(Q, temp->pid, temp->command, temp->priority, temp->execution_time);
                        --(*numProcess);
                        ++(*Qnum);
                    }
                    else enQueue(q, temp->pid, temp->command, temp->priority, temp->execution_time);
                }
                else{
                    --(*numProcess);
                    for(int i = 0; i < historyIndex; i++){
                        if(history[i].pid == temp->pid){
                            history[i].execution_time = temp->execution_time;
                            break;
                        }
                    }
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
}
