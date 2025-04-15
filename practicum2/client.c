#include "client.h"

// Minimal function to send a file to the server in one recv() call
// For a real solution, use a loop for partial writes, or send file size first, etc.
static int send_file_data(int sock, const char *localFile)
{
    FILE *fp = fopen(localFile, "rb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    char buffer[BUF_SIZE];
    int bytes_read = fread(buffer, 1, BUF_SIZE, fp);
    fclose(fp);

    if (bytes_read > 0) {
        // Send file contents
        int sent = send(sock, buffer, bytes_read, 0);
        if (sent < 0) {
            perror("send");
            return -1;
        }
    }
    return 0;
}

/**
 * After sending "WRITE localFile remoteFile",
 *  1. The server should respond "OK_READY_TO_RECEIVE"
 *  2. Then we send the file data
 *  3. Then we read final "WRITE_OK" (or error)
 */
static int handle_write_command(int sock, char *localFile, char *remoteFile)
{
    char cmd_buf[BUF_SIZE];
    memset(cmd_buf, 0, sizeof(cmd_buf));

    // Build command: "WRITE localFile remoteFile"
    snprintf(cmd_buf, sizeof(cmd_buf), "WRITE %s %s", localFile, remoteFile);

    // Send command
    if (send(sock, cmd_buf, strlen(cmd_buf), 0) < 0) {
        perror("send");
        return -1;
    }

    // Wait for "OK_READY_TO_RECEIVE"
    char response[BUF_SIZE] = {0};
    int recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) return -1;
    response[recvd] = '\0';

    if (strncmp(response, "OK_READY_TO_RECEIVE", 19) != 0) {
        fprintf(stderr, "Server response error: %s\n", response);
        return -1;
    }

    // Send the file data
    if (send_file_data(sock, localFile) < 0) {
        return -1;
    }

    // Wait for final "WRITE_OK" from server
    memset(response, 0, sizeof(response));
    recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) return -1;

    response[recvd] = '\0';
    printf("[Client] Server final response: %s\n", response);
    return 0;
}

/**
 * GET: "GET remoteFile localFile"
 *  1. Send command
 *  2. Wait for "OK_SENDING_FILE" or "ERR_FILE_NOT_FOUND"
 *  3. If OK, recv the file data, write to localFile
 */
static int handle_get_command(int sock, char *remoteFile, char *localFile)
{
    char cmd_buf[BUF_SIZE];
    memset(cmd_buf, 0, sizeof(cmd_buf));

    snprintf(cmd_buf, sizeof(cmd_buf), "GET %s %s", remoteFile, localFile);

    // Send command
    if (send(sock, cmd_buf, strlen(cmd_buf), 0) < 0) {
        perror("send");
        return -1;
    }

    // Wait for response
    char response[BUF_SIZE] = {0};
    int recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) return -1;
    response[recvd] = '\0';

    // Check if server said "ERR_FILE_NOT_FOUND"
    if (strncmp(response, "ERR_FILE_NOT_FOUND", 18) == 0) {
        fprintf(stderr, "Server: file not found.\n");
        return -1;
    }

    if (strncmp(response, "OK_SENDING_FILE", 15) != 0) {
        fprintf(stderr, "Server error: %s\n", response);
        return -1;
    }

    // Now the server will send file data immediately
    // For simplicity, do one recv() (not robust for large files)
    char file_data[BUF_SIZE] = {0};
    recvd = recv(sock, file_data, sizeof(file_data), 0);
    if (recvd > 0) {
        FILE *fp = fopen(localFile, "wb");
        if (!fp) {
            perror("fopen");
            return -1;
        }
        fwrite(file_data, 1, recvd, fp);
        fclose(fp);
        printf("[Client] File received and saved to '%s'.\n", localFile);
    }

    return 0;
}

/**
 * RM: "RM remoteFile"
 */
static int handle_rm_command(int sock, char *remoteFile)
{
    char cmd_buf[BUF_SIZE];
    memset(cmd_buf, 0, sizeof(cmd_buf));

    snprintf(cmd_buf, sizeof(cmd_buf), "RM %s", remoteFile);

    // Send command
    if (send(sock, cmd_buf, strlen(cmd_buf), 0) < 0) {
        perror("send");
        return -1;
    }

    // Wait for response
    char response[BUF_SIZE] = {0};
    int recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) return -1;
    response[recvd] = '\0';

    printf("[Client] Server response: %s\n", response);
    return 0;
}


int main(int argc, char *argv[])
{
    // Usage:
    //   rfs WRITE localFile remoteFile
    //   rfs GET remoteFile localFile
    //   rfs RM remoteFile
    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s WRITE <local> <remote>\n", argv[0]);
        fprintf(stderr, "  %s GET   <remote> <local>\n", argv[0]);
        fprintf(stderr, "  %s RM    <remote>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 1. Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. Setup server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);
    // connect to localhost (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // 3. Connect
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("[Client] Connected to server on port %d.\n", PORT);

    // 4. Dispatch command
    int status = 0;
    if (strcasecmp(argv[1], "WRITE") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Not enough arguments for WRITE.\n");
            status = -1;
        } else {
            status = handle_write_command(sock, argv[2], argv[3]);
        }

    } else if (strcasecmp(argv[1], "GET") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Not enough arguments for GET.\n");
            status = -1;
        } else {
            status = handle_get_command(sock, argv[2], argv[3]);
        }

    } else if (strcasecmp(argv[1], "RM") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Not enough arguments for RM.\n");
            status = -1;
        } else {
            status = handle_rm_command(sock, argv[2]);
        }

    } else {
        fprintf(stderr, "Unknown command '%s'\n", argv[1]);
        status = -1;
    }

    // 5. Close the socket
    close(sock);
    return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
