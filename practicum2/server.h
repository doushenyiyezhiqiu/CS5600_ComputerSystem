#ifndef SERVER_H
#define SERVER_H

#include "common.h"

// Function that each client thread will run
void *client_handler(void *arg);

#endif // SERVER_H
