#ifndef POLYBIUS_H
#define POLYBIUS_H

// Polybius Square table structure
typedef struct {
    char grid[5][5];
} PolybiusTable_t;

// Function declarations
char* pbEncode(const char *plaintext, PolybiusTable_t *table);
char* pbDecode(const char *ciphertext, PolybiusTable_t *table);

#endif
