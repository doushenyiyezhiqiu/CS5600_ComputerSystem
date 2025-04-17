#include "client.h"

// Forward declarations of client-side helpers
static int do_write(int sock, char *localFile, char *remoteFile, char *permStr);
static int do_get(int sock, char *remoteFile, char *localFile);
static int do_rm(int sock, char *remoteFile);

int main(int argc, char *argv[])
{
    // Example usage:
    //   rfs WRITE localFile remoteFile [RO|RW]
    //   rfs GET   remoteFile localFile
    //   rfs RM    remoteFile
    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s WRITE <localFile> <remoteFile> [RO|RW]\n", argv[0]);
        fprintf(stderr, "  %s GET   <remoteFile> <localFile>\n", argv[0]);
        fprintf(stderr, "  %s RM    <remoteFile>\n", argv[0]);
        return 1;
    }

    // 1. Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 2. Setup server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);

    // Connect to localhost (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return 1;
    }

    // 3. Connect
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }
    printf("[Client] Connected to server on port %d.\n", PORT);

    // 4. Dispatch command
    int status = 0;
    if (strcasecmp(argv[1], "WRITE") == 0) {
        // Need at least 4 args in total: "WRITE localFile remoteFile [RO|RW]"
        if (argc < 4) {
            fprintf(stderr, "Not enough args for WRITE.\n");
            status = 1;
        } else {
            char *localFile  = argv[2];
            char *remoteFile = argv[3];
            char *permStr    = (argc >= 5) ? argv[4] : NULL;
            status = do_write(sock, localFile, remoteFile, permStr);
        }
    }
    else if (strcasecmp(argv[1], "GET") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Not enough args for GET.\n");
            status = 1;
        } else {
            char *remoteFile = argv[2];
            char *localFile  = argv[3];
            status = do_get(sock, remoteFile, localFile);
        }
    }
    else if (strcasecmp(argv[1], "RM") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Not enough args for RM.\n");
            status = 1;
        } else {
            char *remoteFile = argv[2];
            status = do_rm(sock, remoteFile);
        }
    }
    else {
        fprintf(stderr, "Unknown command '%s'.\n", argv[1]);
        status = 1;
    }

    close(sock);
    return status;
}

// ---------------------------------------------------------------------
// Implementation details
// ---------------------------------------------------------------------

// For a "WRITE localFile remoteFile [RO|RW]" command
static int do_write(int sock, char *localFile, char *remoteFile, char *permStr)
{
    // Construct the command string
    // e.g. "WRITE localFile remoteFile RO"
    char cmd_buf[BUF_SIZE];
    memset(cmd_buf, 0, sizeof(cmd_buf));

    if (permStr) {
        snprintf(cmd_buf, sizeof(cmd_buf), "WRITE %s %s %s", localFile, remoteFile, permStr);
    } else {
        snprintf(cmd_buf, sizeof(cmd_buf), "WRITE %s %s", localFile, remoteFile);
    }

    // Send the command
    if (send(sock, cmd_buf, strlen(cmd_buf), 0) < 0) {
        perror("send");
        return 1;
    }

    // Wait for "OK_READY_TO_RECEIVE" or some error
    char response[BUF_SIZE] = {0};
    int recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) {
        fprintf(stderr, "Server closed connection unexpectedly.\n");
        return 1;
    }
    response[recvd] = '\0';

    if (strncmp(response, "OK_READY_TO_RECEIVE", 19) != 0) {
        fprintf(stderr, "Server error: %s\n", response);
        return 1;
    }

    // Now send the file data
    FILE *fp = fopen(localFile, "rb");
    if (!fp) {
        perror("fopen (localFile)");
        return 1;
    }

    char file_buf[BUF_SIZE];
    int bytes_read = fread(file_buf, 1, BUF_SIZE, fp);
    fclose(fp);

    if (bytes_read > 0) {
        if (send(sock, file_buf, bytes_read, 0) < 0) {
            perror("send file data");
            return 1;
        }
    }

    // Wait for final "WRITE_OK" or error
    memset(response, 0, sizeof(response));
    recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) {
        fprintf(stderr, "No final response from server.\n");
        return 1;
    }
    response[recvd] = '\0';
    printf("[Client] Server final response: %s\n", response);

    return 0;
}

// For a "GET remoteFile localFile" command
static int do_get(int sock, char *remoteFile, char *localFile)
{
    // e.g. "GET folder/remote.txt downloaded.txt"
    char cmd_buf[BUF_SIZE];
    snprintf(cmd_buf, sizeof(cmd_buf), "GET %s %s", remoteFile, localFile);

    if (send(sock, cmd_buf, strlen(cmd_buf), 0) < 0) {
        perror("send");
        return 1;
    }

    // Wait for response (e.g., "OK_SENDING_FILE" or "ERR_FILE_NOT_FOUND")
    char response[BUF_SIZE] = {0};
    int recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) {
        fprintf(stderr, "Server closed connection unexpectedly.\n");
        return 1;
    }
    response[recvd] = '\0';

    if (strncmp(response, "ERR_FILE_NOT_FOUND", 18) == 0) {
        fprintf(stderr, "Server error: File not found.\n");
        return 1;
    }
    if (strncmp(response, "OK_SENDING_FILE", 15) != 0) {
        fprintf(stderr, "Server error: %s\n", response);
        return 1;
    }

    // Now server should send file data
    // Minimal approach: single recv (not robust for big files)
    char file_buf[BUF_SIZE] = {0};
    recvd = recv(sock, file_buf, sizeof(file_buf), 0);
    if (recvd > 0) {
        FILE *fp = fopen(localFile, "wb");
        if (!fp) {
            perror("fopen (localFile)");
            return 1;
        }
        fwrite(file_buf, 1, recvd, fp);
        fclose(fp);
        printf("[Client] File received, written to '%s'\n", localFile);
    } else {
        printf("[Client] File is empty or server didn't send data.\n");
    }

    return 0;
}

// For a "RM remoteFile" command
static int do_rm(int sock, char *remoteFile)
{
    char cmd_buf[BUF_SIZE];
    snprintf(cmd_buf, sizeof(cmd_buf), "RM %s", remoteFile);

    if (send(sock, cmd_buf, strlen(cmd_buf), 0) < 0) {
        perror("send");
        return 1;
    }

    char response[BUF_SIZE] = {0};
    int recvd = recv(sock, response, sizeof(response) - 1, 0);
    if (recvd <= 0) {
        fprintf(stderr, "Server closed connection unexpectedly.\n");
        return 1;
    }
    response[recvd] = '\0';
    printf("[Client] Server response: %s\n", response);

    return 0;
}
