/*
 * multithread.c
 * This file is for Part1 question1.
 * A simple Pthreads example demonstrating thread creation.
 *
 * Compile and run (from the same directory as the Makefile):
 *   make multithread
 *   ./multithread
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <pthread.h>
 
 void* worker(void* arg) {
     char* name = (char*) arg;
     for (int i = 0; i < 5; i++) {
         usleep(50000);  // 50ms
         printf("Hello from thread %s\n", name);
     }
     printf("Thread %s done!\n", name);
     return NULL;
 }
 
 int main(void) {
     pthread_t th1, th2;
 
     // Create two threads
     pthread_create(&th1, NULL, worker, "X");
     pthread_create(&th2, NULL, worker, "Y");
 
     // Let threads run briefly
     sleep(1);
 
     printf("Exiting main program\n");
     return 0;
 }
 