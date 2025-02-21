/*
 * pqueuepmain.c
 * Example Producer-Consumer using a bounded buffer of size 15.
 *
 * Requirements:
 *  - At least 2 producers & 2 consumers
 *  - Stop producing after 30 total items
 *  - Stop consuming after 30 total items
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <unistd.h>
 
 #include "pqueuep.h" // pqueue interface
 
 #define CAPACITY 15
 
 // Global counters
 int totalProduced = 0;
 int totalConsumed = 0;
 pthread_mutex_t gCountLock = PTHREAD_MUTEX_INITIALIZER;
 
 void* producer(void* arg) {
     pqueue_t* pq = (pqueue_t*) arg;
 
     while (1) {
         // Check if we've produced 30
         pthread_mutex_lock(&gCountLock);
         if (totalProduced >= 30) {
             pthread_mutex_unlock(&gCountLock);
             printf("Producer %lu exiting (30 produced)\n", pthread_self());
             break;
         }
         totalProduced++;
         int nextItem = totalProduced;
         pthread_mutex_unlock(&gCountLock);
 
         // Put the item in the queue
         pput(pq, nextItem);
         printf("Producer %lu produced item %d\n", pthread_self(), nextItem);
 
         usleep(100000); // small delay
     }
 
     return NULL;
 }
 
 void* consumer(void* arg) {
     pqueue_t* pq = (pqueue_t*) arg;
 
     while (1) {
         // Check if we've consumed 30
         pthread_mutex_lock(&gCountLock);
         if (totalConsumed >= 30) {
             pthread_mutex_unlock(&gCountLock);
             printf("Consumer %lu exiting (30 consumed)\n", pthread_self());
             break;
         }
         pthread_mutex_unlock(&gCountLock);
 
         // Get an item from the queue
         int item = pget(pq);
 
         pthread_mutex_lock(&gCountLock);
         totalConsumed++;
         int c = totalConsumed;
         pthread_mutex_unlock(&gCountLock);
 
         printf("Consumer %lu consumed item %d (totalConsumed=%d)\n",
                pthread_self(), item, c);
 
         usleep(150000); // small delay
     }
 
     return NULL;
 }
 
 int main(void) {
     // Initialize the protected queue with capacity 15
     pqueue_t* pq = pqueueinit(CAPACITY);
 
     // Create at least 2 producers & 2 consumers
     pthread_t p1, p2, c1, c2;
     pthread_create(&p1, NULL, producer, pq);
     pthread_create(&p2, NULL, producer, pq);
     pthread_create(&c1, NULL, consumer, pq);
     pthread_create(&c2, NULL, consumer, pq);
 
     // Join them
     pthread_join(p1, NULL);
     pthread_join(p2, NULL);
     pthread_join(c1, NULL);
     pthread_join(c2, NULL);
 
     printf("All threads joined. totalProduced=%d, totalConsumed=%d\n",
            totalProduced, totalConsumed);
 
     return 0;
 }
 