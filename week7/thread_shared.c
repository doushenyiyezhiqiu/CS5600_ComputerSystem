/*
 * thread_shared.c
 * This is for Part1 question3.
 * Demonstrates how threads share a global array without synchronization,
 * potentially causing inconsistent reads.
 *
 * Usage:
 *   make thread_shared
 *   ./thread_shared
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <pthread.h>
 
 #define ARR_SIZE 10
 
 // Global array - shared memory
 int shared_array[ARR_SIZE];
 
 // Writer: fill global array with random data
 void* writer_thread(void* arg) {
     pthread_t tid = pthread_self();
     for (int round = 0; round < 5; round++) {
         for (int i = 0; i < ARR_SIZE; i++) {
             shared_array[i] = rand() % 100;
         }
         printf("[Writer %lu] Wrote data.\n", (unsigned long)tid);
         sleep(1);
     }
     return NULL;
 }
 
 // Reader: read and print the global array
 void* reader_thread(void* arg) {
     pthread_t tid = pthread_self();
     for (int round = 0; round < 5; round++) {
         printf("[Reader %lu] Reading data: ", (unsigned long)tid);
         for (int i = 0; i < ARR_SIZE; i++) {
             printf("%d ", shared_array[i]);
         }
         printf("\n");
         usleep(500000); // 0.5s
     }
     return NULL;
 }
 
 int main(void) {
     pthread_t t1, t2;
 
     srand( (unsigned) time(NULL) ); // initialize random seed
 
     pthread_create(&t1, NULL, writer_thread, NULL);
     pthread_create(&t2, NULL, reader_thread, NULL);
 
     pthread_join(t1, NULL);
     pthread_join(t2, NULL);
 
     printf("Main thread done.\n");
     return 0;
 }
 