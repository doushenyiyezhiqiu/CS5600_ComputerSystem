#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>

// Default port and buffer sizes
#define PORT 2024
#define BUF_SIZE 1024

// The top-level folder in which the server stores files
#define SERVER_DATA_DIR "server_data"

// Permissions
typedef enum {
    READ_WRITE,
    READ_ONLY
} permission_t;

#endif // COMMON_H
