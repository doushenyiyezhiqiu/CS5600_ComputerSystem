#include "polybius.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Hardcoded Polybius Square implementation
PolybiusTable_t standardTable = {
    .grid = {
        {'A', 'B', 'C', 'D', 'E'},
        {'F', 'G', 'H', 'I', 'K'},
        {'L', 'M', 'N', 'O', 'P'},
        {'Q', 'R', 'S', 'T', 'U'},
        {'V', 'W', 'X', 'Y', 'Z'}
    }
};

// Function to encode plaintext using the Polybius Square
char* pbEncode(const char *plaintext, PolybiusTable_t *table) {
    char *encoded = malloc(strlen(plaintext) * 2 + 1); // Each char encoded as two digits
    int pos = 0;
    for (int i = 0; plaintext[i] != '\0'; i++) {
        char ch = toupper(plaintext[i]);
        if (ch == 'J') ch = 'I'; // Treat 'J' as 'I'
        for (int row = 0; row < 5; row++) {
            for (int col = 0; col < 5; col++) {
                if (table->grid[row][col] == ch) {
                    encoded[pos++] = '1' + row;
                    encoded[pos++] = '1' + col;
                    break;
                }
            }
        }
    }
    encoded[pos] = '\0';
    return encoded;
}

// Function to decode ciphertext using the Polybius Square
char* pbDecode(const char *ciphertext, PolybiusTable_t *table) {
    int len = strlen(ciphertext) / 2;
    char *decoded = malloc(len + 1);
    int pos = 0;
    for (int i = 0; ciphertext[i] != '\0' && ciphertext[i+1] != '\0'; i += 2) {
        int row = ciphertext[i] - '1';
        int col = ciphertext[i+1] - '1';
        decoded[pos++] = table->grid[row][col];
    }
    decoded[pos] = '\0';
    return decoded;
}
