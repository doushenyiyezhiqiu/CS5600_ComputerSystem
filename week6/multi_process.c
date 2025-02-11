/******************************************************************************
 * multi_process.c
 *
 * Implements the new assignment requirements:
 *   1) Generate 10,000 random strings and write them to "strings.txt".
 *   2) Read "strings.txt" into a queue.
 *   3) Process in chunks of 100, spawning child processes that run:
 *      ./cipher chunk_i.txt
 *      and redirect each to cipher_output_i.txt
 *
 ******************************************************************************/
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <unistd.h>     // fork(), execlp(), etc.
 #include <sys/wait.h>   // wait()
 #include <ctype.h>
 #include "queue.h"
 
 /*============================================================================
   Simple Linear Congruential Generator (LCG) to produce pseudo-random numbers.
   This meets my prior assignment's requirement of "not using <stdlib>'s rand".
 ============================================================================*/
 static unsigned long lcg_state = 1UL;
 
 // We can pick some standard 32-bit LCG constants
 unsigned long lcg_rand(void) {
     // X_{n+1} = (1103515245 * X_n + 12345) mod 2^31
     lcg_state = (1103515245UL * lcg_state + 12345UL) & 0x7fffffffUL;
     return lcg_state;
 }
 
 // Return an integer in [minVal, maxVal]
 int lcg_rand_range(int minVal, int maxVal) {
     unsigned long r = lcg_rand();
     int range = maxVal - minVal + 1;
     return (int)(r % range) + minVal;
 }
 
 /*============================================================================
   Generate a random string with length in [minLen, maxLen], containing letters.
 ============================================================================*/
 void generateRandomString(char *buffer, int minLen, int maxLen) {
     int length = lcg_rand_range(minLen, maxLen);
     for (int i = 0; i < length; i++) {
         // pick a random letter from 'a'..'z'
         buffer[i] = (char)lcg_rand_range('a', 'z');
     }
     buffer[length] = '\0';
 }
 
 /*============================================================================
   PART 1: Generate 10,000 random strings and write them to "strings.txt"
 ============================================================================*/
 void generateStringsFile(const char *filename) {
     FILE *fp = fopen(filename, "w");
     if (!fp) {
         perror("fopen for write");
         exit(EXIT_FAILURE);
     }
 
     const int totalStrings = 10000;
     for (int i = 0; i < totalStrings; i++) {
         char temp[32]; // buffer for a random string up to ~31 chars
         generateRandomString(temp, 5, 10);
         fprintf(fp, "%s\n", temp);
     }
 
     fclose(fp);
     printf("[INFO] Generated %d random strings in '%s'\n", totalStrings, filename);
 }
 
 /*============================================================================
   PART 2: Read the file "strings.txt" line by line and enqueue them.
 ============================================================================*/
 void loadStringsIntoQueue(const char *filename, Queue *q) {
     FILE *fp = fopen(filename, "r");
     if (!fp) {
         perror("fopen for read");
         exit(EXIT_FAILURE);
     }
 
     char line[256];
     while (fgets(line, sizeof(line), fp)) {
         // remove trailing newline
         size_t len = strlen(line);
         if (len > 0 && line[len - 1] == '\n') {
             line[len - 1] = '\0';
         }
         enqueue(q, line);
     }
 
     fclose(fp);
     printf("[INFO] Loaded %d lines into the queue.\n", sizeQueue(q));
 }
 
 /*============================================================================
   Write up to 'count' items from the queue to a "chunk_i.txt" file.
   Returns how many strings were actually written.
 ============================================================================*/
 int writeChunkToFile(Queue *q, int count, const char *chunkFile) {
     FILE *fp = fopen(chunkFile, "w");
     if (!fp) {
         perror("fopen chunkFile");
         return 0;
     }
 
     int written = 0;
     while (!isEmptyQueue(q) && written < count) {
         char *str = dequeue(q);
         fprintf(fp, "%s\n", str);
         free(str); // free the copy from the queue
         written++;
     }
 
     fclose(fp);
     return written;
 }
 
 /*============================================================================
   PART 3: For each chunk of 100 strings:
     - Write them to chunk_i.txt
     - fork() and in the child:
        * redirect stdout to cipher_output_i.txt
        * execlp("./cipher", "cipher", "chunk_i.txt", NULL);
 ============================================================================*/
 void processInChunks(Queue *q, int chunkSize) {
     int chunkIndex = 0;
     while (!isEmptyQueue(q)) {
         char chunkFilename[64];
         sprintf(chunkFilename, "chunk_%d.txt", chunkIndex);
 
         int actualCount = writeChunkToFile(q, chunkSize, chunkFilename);
         if (actualCount == 0) {
             // no more strings left
             break;
         }
 
         // fork and run ./cipher chunkFilename -> cipher_output_chunkIndex.txt
         pid_t pid = fork();
         if (pid < 0) {
             perror("fork");
             exit(EXIT_FAILURE);
         } else if (pid == 0) {
             // Child process
             char outputFilename[64];
             sprintf(outputFilename, "cipher_output_%d.txt", chunkIndex);
 
             // Redirect stdout to outputFilename
             FILE *outfp = freopen(outputFilename, "w", stdout);
             if (!outfp) {
                 perror("freopen");
                 exit(EXIT_FAILURE);
             }
 
             // exec "./cipher chunkFilename"
             execlp("./cipher", "cipher", chunkFilename, (char *)NULL);
 
             // if execlp returns, there's an error
             perror("execlp");
             exit(EXIT_FAILURE);
         } else {
             // parent process waits for child (sequential approach)
             wait(NULL);
         }
 
         chunkIndex++;
     }
 
     printf("[INFO] All strings processed in %d chunks.\n", chunkIndex);
 }
 
 /*============================================================================
   main()
     - "generate": produce strings.txt
     - "encrypt": read strings.txt into a queue, chunk process with ./cipher
 ============================================================================*/
 int main(int argc, char *argv[]) {
     if (argc < 2) {
         fprintf(stderr, "Usage: %s [generate|encrypt]\n", argv[0]);
         return EXIT_FAILURE;
     }
 
     // Initialize our LCG seed with current time
     lcg_state = (unsigned long)time(NULL);
 
     if (strcmp(argv[1], "generate") == 0) {
         generateStringsFile("strings.txt");
     } 
     else if (strcmp(argv[1], "encrypt") == 0) {
         Queue q;
         initQueue(&q);
         loadStringsIntoQueue("strings.txt", &q);
 
         // chunk size = 100
         processInChunks(&q, 100);
         
         // free any leftover in queue
         freeQueue(&q);
     } 
     else {
         fprintf(stderr, "Unknown command '%s'. Use 'generate' or 'encrypt'.\n", argv[1]);
         return EXIT_FAILURE;
     }
 
     return EXIT_SUCCESS;
 }
 