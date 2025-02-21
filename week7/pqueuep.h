/*
 * pqueuep.h
 * Interface for a protected, thread-safe queue.
 */

 #ifndef PQUEUEP_H
 #define PQUEUEP_H
 
 // Let's define a type pqueue_t as a struct (in pqueuep.c)
 typedef struct pqueue pqueue_t;
 
 // Initialize a pqueue with given capacity
 pqueue_t* pqueueinit(int capacity);
 
 // Put an integer into the queue (blocks if full)
 void pput(pqueue_t* pq, int value);
 
 // Get an integer from the queue (blocks if empty)
 int pget(pqueue_t* pq);
 
 #endif
 