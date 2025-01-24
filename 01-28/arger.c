/*
* arger.c / Practice Assignment
*
* Liangshe Li / CS5600 / Northeastern University
* Spring 2025 / Jan 21, 2025
*
*/

#include <stdio.h>

// Manually convert character to uppercase
char my_toupper(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}

// Manually convert character to lowercase
char my_tolower(char c) {
    if (c >= 'A' && c <= 'Z')
        return c + 32;
    return c;
}

// Custom function to capitalize the first letter of each word
void to_capitalized(char* str) {
    int newWord = 1;
    for (int i = 0; str[i]; i++) {
        if (newWord && ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))) {
            str[i] = my_toupper(str[i]);
            newWord = 0;
        } else {
            str[i] = my_tolower(str[i]);
        }
        if (str[i] == ' ') newWord = 1;
    }
}

// Function to manually compute string length
size_t my_strlen(const char* str) {
    const char* s;
    for (s = str; *s; ++s) {}
    return (s - str);
}

// Manual strcat function
void my_strcat(char* dest, const char* src) {
    while (*dest) dest++;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

// Manual strcmp function
int my_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -flag \"text\"\n", argv[0]);
        return -1;
    }

    char *flag = argv[1];
    char output[1000] = {0};

    for (int i = 2; i < argc; i++) {
        my_strcat(output, argv[i]);
        my_strcat(output, " ");
    }
    output[my_strlen(output) - 1] = '\0';  // Remove trailing space using custom my_strlen

    if (my_strcmp(flag, "-u") == 0) {
        for (int i = 0; output[i]; i++) output[i] = my_toupper(output[i]);
    } else if (my_strcmp(flag, "-l") == 0) {
        for (int i = 0; output[i]; i++) output[i] = my_tolower(output[i]);
    } else if (my_strcmp(flag, "-cap") == 0) {
        to_capitalized(output);
    } else {
        fprintf(stderr, "Invalid flag '%s'. Use -u for uppercase, -l for lowercase, or -cap for capitalized.\n", flag);
        return -1;
    }

    printf("%s\n", output);
    return 0;
}
