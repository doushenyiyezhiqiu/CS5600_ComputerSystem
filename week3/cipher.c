#include "polybius.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern PolybiusTable_t standardTable; // Use the standard table defined in polybius.c

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: %s -e|-d 'text'\n", argv[0]);
        return 1;
    }

    char *result;
    if (strcmp(argv[1], "-e") == 0) {
        result = pbEncode(argv[2], &standardTable);
        printf("Encoded: %s\n", result);
    } else if (strcmp(argv[1], "-d") == 0) {
        result = pbDecode(argv[2], &standardTable);
        printf("Decoded: %s\n", result);
    } else {
        fprintf(stderr, "Invalid option. Use -e for encode or -d for decode.\n");
        return 1;
    }
    free(result);
    return 0;
}
