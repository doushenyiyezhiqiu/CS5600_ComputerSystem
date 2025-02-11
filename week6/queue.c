#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

void initQueue(Queue *q) {
    q->front = NULL;
    q->rear  = NULL;
    q->size  = 0;
}

void enqueue(Queue *q, const char *str) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("malloc for newNode");
        exit(EXIT_FAILURE);
    }
    newNode->next = NULL;

    // Copy string
    size_t len = strlen(str);
    newNode->data = (char*)malloc(len + 1);
    if (!newNode->data) {
        perror("malloc for data");
        exit(EXIT_FAILURE);
    }
    strcpy(newNode->data, str);

    if (q->rear == NULL) {
        // empty queue
        q->front = newNode;
        q->rear  = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->size++;
}

char* dequeue(Queue *q) {
    if (q->front == NULL) {
        return NULL; // empty
    }
    Node *temp = q->front;
    char *data = temp->data;  // We'll return this pointer
    q->front = temp->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    q->size--;
    return data;
}

int isEmptyQueue(Queue *q) {
    return (q->size == 0);
}

int sizeQueue(Queue *q) {
    return q->size;
}

void freeQueue(Queue *q) {
    while (!isEmptyQueue(q)) {
        char *data = dequeue(q);
        if (data) free(data);
    }
}
