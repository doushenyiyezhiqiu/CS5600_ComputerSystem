/*
 * pqueuep.c
 * Implementation of a thread-safe bounded buffer using a condition variable.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include "pqueuep.h"
 #include "queuep.h" // for the underlying circular buffer struct
 
 struct pqueue {
     queue_t* q;                // underlying circular buffer
     pthread_mutex_t mutex;     // to protect access
     pthread_cond_t cond;       // condition variable for producers & consumers
     int capacity;
 };
 
 // Initialize
 pqueue_t* pqueueinit(int capacity) {
     pqueue_t* pq = (pqueue_t*)malloc(sizeof(pqueue_t));
     pq->q = queue_init(capacity);
     pq->capacity = capacity;
 
     pthread_mutex_init(&pq->mutex, NULL);
     pthread_cond_init(&pq->cond, NULL);
 
     return pq;
 }
 
 void pput(pqueue_t* pq, int value) {
     pthread_mutex_lock(&pq->mutex);
 
     // Wait while the underlying queue is full
     while (queue_full(pq->q)) {
         pthread_cond_wait(&pq->cond, &pq->mutex);
     }
 
     // Insert the item
     queue_put(pq->q, value);
 
     // Debug print (optional)
     // printf("[pput] Inserted %d, size now %d\n", value, queue_size(pq->q));
 
     // Signal the condition so a waiting consumer might proceed
     pthread_cond_broadcast(&pq->cond);
 
     pthread_mutex_unlock(&pq->mutex);
 }
 
 int pget(pqueue_t* pq) {
     pthread_mutex_lock(&pq->mutex);
 
     // Wait while empty
     while (queue_empty(pq->q)) {
         pthread_cond_wait(&pq->cond, &pq->mutex);
     }
 
     // Remove an item
     int value = queue_get(pq->q);
 
     // Debug print (optional)
     // printf("[pget] Got %d, size now %d\n", value, queue_size(pq->q));
 
     // Signal so a waiting producer can proceed
     pthread_cond_broadcast(&pq->cond);
 
     pthread_mutex_unlock(&pq->mutex);
 
     return value;
 }
 