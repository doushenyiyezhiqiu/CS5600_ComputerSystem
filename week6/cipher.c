/******************************************************************************
 * cipher.c
 *
 * A stub cipher that:
 *   - Reads lines from the file specified by argv[1]
 *   - Performs a trivial Caesar shift by +3
 *   - Prints the encrypted lines to stdout
 *
 ******************************************************************************/
 #include <stdio.h>
 #include <stdlib.h>
 #include <ctype.h>
 
 int main(int argc, char *argv[]) {
     if(argc < 2) {
         fprintf(stderr, "Usage: %s <inputfile>\n", argv[0]);
         return EXIT_FAILURE;
     }
 
     FILE *fp = fopen(argv[1], "r");
     if(!fp) {
         perror("cipher: fopen");
         return EXIT_FAILURE;
     }
 
     char line[256];
     while(fgets(line, sizeof(line), fp)) {
         // For each character, shift by +3 if it's alpha
         for(int i = 0; line[i] != '\0'; i++) {
             if(isalpha((unsigned char)line[i])) {
                 // naive Caesar shift
                 char base = (line[i] >= 'a' && line[i] <= 'z') ? 'a' : 'A';
                 line[i] = (char)(base + ((line[i] - base + 3) % 26));
             }
         }
         // Print to stdout
         fputs(line, stdout);
     }
 
     fclose(fp);
     return EXIT_SUCCESS;
 }
 