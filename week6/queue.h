#ifndef QUEUE_H
#define QUEUE_H

// Simple linked-list queue to store strings

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
    int size;
} Queue;

// Initialize queue
void initQueue(Queue *q);

// Add a copy of 'str' to the queue
void enqueue(Queue *q, const char *str);

// Remove and return the front string from the queue (caller must free)
char* dequeue(Queue *q);

// Check if empty
int isEmptyQueue(Queue *q);

// Return queue size
int sizeQueue(Queue *q);

// Free any remaining nodes and reset the queue
void freeQueue(Queue *q);

#endif
