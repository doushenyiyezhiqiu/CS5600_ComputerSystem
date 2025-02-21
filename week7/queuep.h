/*
 * queuep.h
 * Interface for a simple circular queue of integers.
 */

 #ifndef QUEUEP_H
 #define QUEUEP_H
 
 typedef struct queue queue_t;
 
 queue_t* queue_init(int capacity);
 void     queue_put(queue_t* q, int value);
 int      queue_get(queue_t* q);
 int      queue_empty(queue_t* q);
 int      queue_full(queue_t* q);
 int      queue_size(queue_t* q);
 
 #endif
 