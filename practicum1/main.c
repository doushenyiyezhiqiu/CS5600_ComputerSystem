/****************************************************
 * main.c
 * Demonstration / test harness for the message store
 * and LRU caching system.
 ****************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 #include <sys/stat.h>
 #include "message.h"
 #include "cache.h"
 
 void ensure_message_folder() {
     struct stat st = {0};
     if (stat("messages", &st) == -1) {
         mkdir("messages", 0700);
     }
 }
 
 int main(void)
 {
     srand((unsigned int)time(NULL));
     ensure_message_folder();
 
     printf("Creating and storing sample messages (IDs 1..20) on disk...\n");
 
     /* Create a few messages and store them on disk */
     for (int i = 1; i <= 20; i++) {
         char sender[32], receiver[32], content[64];
         snprintf(sender,   sizeof(sender),   "sender%d",   i);
         snprintf(receiver, sizeof(receiver), "receiver%d", i);
         snprintf(content,  sizeof(content),  "Hello, this is message %d!", i);
 
         Message* msg = create_msg(i, sender, receiver, content, 0);
         if (!msg) {
             fprintf(stderr, "Failed to create message ID %d.\n", i);
             continue;
         }
         if (store_msg(msg) != 0) {
             fprintf(stderr, "Failed to store message ID %d.\n", i);
         }
         free(msg);
     }
     printf("Finished storing messages.\n\n");
 
     /* Initialize cache with true LRU logic */
     printf("Initializing cache with true LRU policy...\n");
     init_cache();
 
     CacheStats stats;
     printf("Testing 1000 random accesses on message IDs [1..20]...\n");
     test_cache(1000, 20, &stats);
     print_cache_stats(&stats);
 
     printf("\nDone.\n");
     return 0;
 }
 