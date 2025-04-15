#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>  // for toupper/tolower if needed

// Shared constants
#define PORT 2024
#define BUF_SIZE 1024
#define SERVER_DATA_DIR "server_data"  // Folder on server to store files

#endif // COMMON_H
