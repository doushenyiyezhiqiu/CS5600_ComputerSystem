#include <stdio.h>
#include <stdlib.h>

// Static segment: 5 MB of static memory
static char static_memory[5 * 1024 * 1024]; // 5 MB

int main() {
    // Heap: Allocate 10 MB of memory
    char* heap_memory = (char*)malloc(10 * 1024 * 1024); // 10 MB
    if (heap_memory == NULL) {
        fprintf(stderr, "Heap allocation failed\n");
        return 1;
    }
    printf("10 MB allocated from the heap\n");

    // Stack: Allocate 1 MB of memory
    char stack_memory[1 * 1024 * 1024]; // 1 MB
    printf("1 MB allocated from the stack\n");

    // Use the allocated memory to avoid warnings
    for (int i = 0; i < 5 * 1024 * 1024; i++) {
        static_memory[i] = i % 256; // Fill static memory with some data
    }
    for (int i = 0; i < 10 * 1024 * 1024; i++) {
        heap_memory[i] = i % 256; // Fill heap memory with some data
    }
    for (int i = 0; i < 1 * 1024 * 1024; i++) {
        stack_memory[i] = i % 256; // Fill stack memory with some data
    }

    // Use stack_memory to avoid the "set but not used" warning
    int stack_sum = 0;
    for (int i = 0; i < 1 * 1024 * 1024; i++) {
        stack_sum += stack_memory[i];
    }
    printf("Sum of stack_memory values: %d\n", stack_sum);

    // Memory is now allocated and used
    printf("Memory allocated and initialized: 10 MB (heap), 5 MB (static), 1 MB (stack)\n");

    // Free heap memory
    free(heap_memory);
    printf("10 MB heap memory freed\n");

    // Stack and static memory are automatically released when the program exits
    printf("Program exiting...\n");

    return 0;
}