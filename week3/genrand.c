/*
* genrand.c / Random Number Generation and Encryption
*
* Liangshe Li / CS5600 / Northeastern University
* Spring 2025 / Jan 23, 2025
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

unsigned int seed;  // Global seed for the LCG

// Initialize the seed based on the current time
void initSeed() {
    seed = (unsigned int)time(NULL);
}

// Linear Congruential Generator (LCG) function
unsigned int lcg() {
    const unsigned int a = 1664525;
    const unsigned int c = 1013904223;
    seed = a * seed + c;  // Automatic overflow by the nature of unsigned arithmetic
    return seed;
}

// Generate a random integer between min and max (inclusive)
int genRand(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;  // Swap to ensure min is always less than max
    }
    return (lcg() % (max - min + 1)) + min;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <number of randoms> <filename> [-a]\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    char *filename = argv[2];
    int append = argc == 4 && strcmp(argv[3], "-a") == 0;

    FILE *fp = fopen(filename, append ? "a" : "w");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    initSeed();  // Initialize the seed

    for (int i = 0; i < n; i++) {
        int random_number = genRand(1, 10000);  // Generate random numbers between 1 and 100
        fprintf(fp, "%d\n", random_number);
    }

    fclose(fp);
    return 0;
}
