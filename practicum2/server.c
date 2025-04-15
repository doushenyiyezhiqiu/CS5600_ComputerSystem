#include "server.h"
#include <sys/stat.h> // for mkdir, stat

// Helper: Ensure server_data folder exists (and subfolders if you want)
static void ensure_server_data_dir(void)
{
    // Minimal example: just ensure top-level folder exists
    // For production, you'd want to create subdirectories, etc.
    mkdir(SERVER_DATA_DIR, 0777); // create if not exists
}

/**
 * Write handler:
 *  - The client will send a file to be stored as 'SERVER_DATA_DIR/remotePath'
 *  - For simplicity, this example:
 *    1) Sends "Ready" to client
 *    2) Receives file contents in one shot (ignoring partial reads)
 *    3) Writes data to file
 */
void handle_write(int client_sock, const char *localPath, const char *remotePath)
{
    char buffer[BUF_SIZE] = {0};
    char fullpath[BUF_SIZE] = {0};

    // Build the full server file path
    snprintf(fullpath, sizeof(fullpath), "%s/%s", SERVER_DATA_DIR, remotePath);

    // Let the client know we are ready to receive
    const char *msg = "OK_READY_TO_RECEIVE";
    send(client_sock, msg, strlen(msg), 0);

    // Receive file data from client
    // (In a real solution, you'd probably first recv a file size or loop in chunks)
    int bytes_recv = recv(client_sock, buffer, BUF_SIZE, 0);
    if (bytes_recv <= 0) {
        fprintf(stderr, "Error receiving file data.\n");
        return;
    }

    // Write data to the file on server
    FILE *fp = fopen(fullpath, "wb");
    if (!fp) {
        perror("fopen");
        return;
    }

    fwrite(buffer, 1, bytes_recv, fp);
    fclose(fp);

    // Send success message back
    const char *resp = "WRITE_OK";
    send(client_sock, resp, strlen(resp), 0);
}

void handle_get(int client_sock, const char *remotePath, const char *localPath)
{
    char buffer[BUF_SIZE] = {0};
    char fullpath[BUF_SIZE] = {0};

    // Build the full server file path
    snprintf(fullpath, sizeof(fullpath), "%s/%s", SERVER_DATA_DIR, remotePath);

    // Attempt to open the file
    FILE *fp = fopen(fullpath, "rb");
    if (!fp) {
        // Send error message
        const char *err = "ERR_FILE_NOT_FOUND";
        send(client_sock, err, strlen(err), 0);
        return;
    }

    // Send "OK" to let client know we will send data
    const char *msg = "OK_SENDING_FILE";
    send(client_sock, msg, strlen(msg), 0);

    // Read file data
    int bytes_read = fread(buffer, 1, BUF_SIZE, fp);
    fclose(fp);

    // Send file data in one shot (for brevity)
    // For larger files, you'd loop over fread() + send() in chunks
    if (bytes_read > 0) {
        send(client_sock, buffer, bytes_read, 0);
    }

    // (Optionally) send final success message
    // send(client_sock, "GET_OK", 6, 0);
}

void handle_rm(int client_sock, const char *targetPath)
{
    char fullpath[BUF_SIZE] = {0};

    snprintf(fullpath, sizeof(fullpath), "%s/%s", SERVER_DATA_DIR, targetPath);

    // Attempt to remove the file
    if (remove(fullpath) == 0) {
        const char *msg = "RM_OK";
        send(client_sock, msg, strlen(msg), 0);
    } else {
        const char *err = "ERR_REMOVE_FAILED";
        send(client_sock, err, strlen(err), 0);
    }
}

int main(void)
{
    ensure_server_data_dir();

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUF_SIZE];

    // 1. Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("[Server] Socket created.\n");

    // 2. Bind
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("[Server] Bound to port %d.\n", PORT);

    // 3. Listen
    if (listen(server_sock, 5) == 0) {
        printf("[Server] Listening...\n");
    } else {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 4. Accept incoming connection
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }
        printf("[Server] Client connected.\n");

        // 5. Receive command line from client
        memset(buffer, 0, BUF_SIZE);
        ssize_t bytesRead = recv(client_sock, buffer, BUF_SIZE - 1, 0);
        if (bytesRead <= 0) {
            close(client_sock);
            continue;
        }
        buffer[bytesRead] = '\0'; // Ensure null-termination

        // 6. Parse the command
        //    e.g., "WRITE localFile remoteFile"
        char *cmd = strtok(buffer, " \t\r\n");
        if (!cmd) {
            close(client_sock);
            continue;
        }

        if (strcasecmp(cmd, "WRITE") == 0) {
            char *localPath  = strtok(NULL, " \t\r\n");
            char *remotePath = strtok(NULL, " \t\r\n");
            if (localPath && remotePath) {
                handle_write(client_sock, localPath, remotePath);
            } else {
                const char *err = "ERR_BAD_ARGS";
                send(client_sock, err, strlen(err), 0);
            }

        } else if (strcasecmp(cmd, "GET") == 0) {
            char *remotePath = strtok(NULL, " \t\r\n");
            char *localPath  = strtok(NULL, " \t\r\n");
            if (remotePath && localPath) {
                handle_get(client_sock, remotePath, localPath);
            } else {
                const char *err = "ERR_BAD_ARGS";
                send(client_sock, err, strlen(err), 0);
            }

        } else if (strcasecmp(cmd, "RM") == 0) {
            char *path = strtok(NULL, " \t\r\n");
            if (path) {
                handle_rm(client_sock, path);
            } else {
                const char *err = "ERR_BAD_ARGS";
                send(client_sock, err, strlen(err), 0);
            }
        } else {
            // Unknown command
            const char *err = "ERR_UNKNOWN_CMD";
            send(client_sock, err, strlen(err), 0);
        }

        // 7. Close this client connection, then wait for the next
        close(client_sock);
        printf("[Server] Client disconnected.\n");
    }

    // 8. Close listening socket (though we never reach here in this example)
    close(server_sock);
    return 0;
}
