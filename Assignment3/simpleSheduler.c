// A C program to demonstrate linked list based
// implementation of queue
#include <stdio.h>
#include <stdlib.h>

// A linked list (LL) node to store a queue entry
typedef struct QNode {
	int key;
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
	temp->key = k;
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

// The function to add a key k to q
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

// Function to remove a key from given queue q
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
        printf("%d ", current->key);
        current = current->next;
    }
    printf("\n");
}

int main() {
    struct Queue* q = createQueue();
    enQueue(q, 10);
    enQueue(q, 20);
    // deQueue(q);
    // deQueue(q);
    enQueue(q, 30);
    enQueue(q, 40);
    enQueue(q, 50);
    // deQueue(q);

    // Print all elements in the queue
    printQueue(q);

    return 0;
}
