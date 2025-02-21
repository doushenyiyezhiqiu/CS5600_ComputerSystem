/*
 * thread_cancel.c
 * This is for Part1 question 2.
 * Demonstrates cancelling a thread, printing out thread IDs.
 *
 * Usage:
 *   make thread_cancel
 *   ./thread_cancel
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <unistd.h>
 
 void* do_work(void* arg) {
     pthread_t tid = pthread_self();
     char* name = (char*) arg;
 
     printf("Thread %s started (ID: %lu)\n", name, (unsigned long)tid);
 
     // Work for a while
     for (int i = 0; i < 10; i++) {
         printf("Thread %s (ID: %lu) working...\n", name, (unsigned long)tid);
         sleep(1);
     }
     printf("Thread %s (ID: %lu) completed normally.\n", name, (unsigned long)tid);
     return NULL;
 }
 
 int main(void) {
     pthread_t t1, t2;
 
     pthread_create(&t1, NULL, do_work, "X");
     pthread_create(&t2, NULL, do_work, "Y");
 
     sleep(3); // Let them run a few seconds
 
     // Now cancel thread Y
     printf("====> Cancelling Thread Y (pthread_t: %lu)\n", (unsigned long)t2);
     pthread_cancel(t2);
 
     // Wait for threads to finish
     pthread_join(t1, NULL);
     pthread_join(t2, NULL);
 
     printf("Main thread done.\n");
     return 0;
 }
 