#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* (3 pts) Define a type process_t */
typedef struct process_s {
    int   id;
    char *name;
    long  runtime;
    int   priority;
} process_t;

/* Node structure for our doubly-linked list queue. */
typedef struct node_s {
    void             *data;
    struct node_s    *prev;
    struct node_s    *next;
} node_t;

/* (2 pts) Define a type queue_t that represents a queue that can hold anything */
typedef struct queue_s {
    node_t *head;
    node_t *tail;
    int     size;
} queue_t;

/* ----- Function Prototypes ----- */

/* Creates and returns an empty queue */
queue_t *create_queue(void);

/* (5 pts) push_queue: add element to end of the queue */
void push_queue(queue_t *queue, void *element);

/* (5 pts) pop_queue: remove and return data at the front of the queue */
void *pop_queue(queue_t *queue);

/* (5 pts) remove_process: remove and return the process with the highest priority */
process_t *remove_process(queue_t *queue);

/* (5 pts) get_queue_size: returns number of elements in queue */
int get_queue_size(queue_t *queue);

/* Helper to destroy a queue (optional, but often good practice) */
void destroy_queue(queue_t *queue);

#endif
