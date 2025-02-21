/*
 * queuep.c
 * A simple unprotected circular queue of integers.
 * This is used internally by the protected pqueue.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "queuep.h"
 
 struct queue {
     int*  data;
     int   head;
     int   tail;
     int   size;       // current number of elements
     int   capacity;
 };
 
 queue_t* queue_init(int capacity) {
     queue_t* q = (queue_t*)malloc(sizeof(queue_t));
     q->data = (int*)malloc(sizeof(int) * capacity);
     q->head = 0;
     q->tail = 0;
     q->size = 0;
     q->capacity = capacity;
     return q;
 }
 
 void queue_put(queue_t* q, int value) {
     if (q->size < q->capacity) {
         q->data[q->tail] = value;
         q->tail = (q->tail + 1) % q->capacity;
         q->size++;
     }
     // else ignore or handle error
 }
 
 int queue_get(queue_t* q) {
     int result = -1;
     if (q->size > 0) {
         result = q->data[q->head];
         q->head = (q->head + 1) % q->capacity;
         q->size--;
     }
     return result;
 }
 
 int queue_empty(queue_t* q) {
     return (q->size == 0);
 }
 
 int queue_full(queue_t* q) {
     return (q->size == q->capacity);
 }
 
 int queue_size(queue_t* q) {
     return q->size;
 }
 