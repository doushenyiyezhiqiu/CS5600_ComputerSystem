#ifndef SERVER_H
#define SERVER_H

#include "common.h"

// Function prototypes for request handlers
void handle_write(int client_sock, const char *localPath, const char *remotePath);
void handle_get(int client_sock, const char *remotePath, const char *localPath);
void handle_rm(int client_sock, const char *targetPath);

#endif // SERVER_H
