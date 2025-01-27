#include <stdio.h>
#include <stdlib.h>

// Define the dynamic block structure
typedef struct {
    int* data;       // Pointer to dynamically allocated array of integers
    size_t size;     // Size of the array
} dyn_block;

// Function to allocate a dynamic block
dyn_block* alloc_dyn_block(size_t size) {
    dyn_block* block = (dyn_block*)malloc(sizeof(dyn_block));
    if (block == NULL) {
        fprintf(stderr, "Failed to allocate dyn_block\n");
        return NULL;
    }
    block->data = (int*)malloc(size * sizeof(int));
    if (block->data == NULL) {
        fprintf(stderr, "Failed to allocate data array\n");
        free(block);
        return NULL;
    }
    block->size = size;
    return block;
}

// Function to store an array of integers in a dynamic block
void store_mem_blk(dyn_block* block, int* array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        block->data[i] = array[i];
    }
}

// Function to print a dynamic block
void print_dyn_block(dyn_block* block) {
    for (size_t i = 0; i < block->size; i++) {
        printf("%d ", block->data[i]);
    }
    printf("\n");
}

// Function to free a dynamic block
void free_dyn_block(dyn_block* block) {
    free(block->data);
    free(block);
}

int main() {
    FILE* file = fopen("blocks.data", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // Read each line from the file
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Count the number of integers in the line
        size_t count = 0;
        int value;
        char* ptr = line;
        while (sscanf(ptr, "%d", &value) == 1) {
            count++;
            while (*ptr && *ptr != ' ') ptr++; // Move to the next number
            while (*ptr == ' ') ptr++;        // Skip spaces
        }

        // Allocate a dynamic block for the integers
        dyn_block* block = alloc_dyn_block(count);
        if (block == NULL) {
            fprintf(stderr, "Failed to allocate block\n");
            fclose(file);
            return 1;
        }

        // Read the integers into the block
        ptr = line;
        for (size_t i = 0; i < count; i++) {
            sscanf(ptr, "%d", &block->data[i]);
            while (*ptr && *ptr != ' ') ptr++; // Move to the next number
            while (*ptr == ' ') ptr++;        // Skip spaces
        }

        // Print the block to demonstrate correctness
        print_dyn_block(block);

        // Free the block
        free_dyn_block(block);
    }

    fclose(file);
    return 0;
}