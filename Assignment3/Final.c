// A C program to demonstrate linked list based
// implementation of queue
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


// A linked list (LL) node to store a queue entry
typedef struct QNode {
	int pid;
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
void deQueue(struct Queue* q)
{
	// If queue is empty, return NULL.
	if (q->front == NULL)
		return;

	// Store previous front and move front one node ahead
	QNode* temp = q->front;

	q->front = q->front->next;

	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
		q->rear = NULL;

	free(temp);
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

int main(){
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
        
        else if(strncmp(input, "submit", 6) != 0){
            printf("Wrong Input Method!\n");
        }

        else printf("%s\n", input);
    }




    return 0;
}