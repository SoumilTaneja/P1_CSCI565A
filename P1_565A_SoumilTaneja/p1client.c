#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 3000

void error_handling(const char *message);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    int str_len;
    
    // Validate the number of arguments
    if ((strcmp(argv[4], "send") == 0 && argc != 6) || (strcmp(argv[4], "receive") == 0 && argc != 5)) {
        printf("Usage: %s <IP> <port> <TCP|UDP> <send|receive> [<file>]\n", argv[0]);
        exit(1);
    }

    // Determine socket type
    int socket_type;
    if (strcmp(argv[3], "TCP") == 0) {
        socket_type = SOCK_STREAM;
    } else if (strcmp(argv[3], "UDP") == 0) {
        socket_type = SOCK_DGRAM;
    } else {
        error_handling("Invalid socket type. Use 'TCP' or 'UDP'.");
    }

    // Create socket
    sock = socket(PF_INET, socket_type, 0);
    if (sock == -1)
        error_handling("socket() error");

    // Configure server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // Connect for TCP
    if (socket_type == SOCK_STREAM) {
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
            error_handling("connect() error");
    }

    if (strcmp(argv[4], "send") == 0) {
        // Open the file to send
        FILE *fp = fopen(argv[5], "r");
        if (!fp)
            error_handling("fopen() error");

        // Read file contents into the message buffer
        size_t n_read = fread(message, sizeof(char), BUF_SIZE - 1, fp);
        fclose(fp);

        if (n_read == 0)
            error_handling("Empty file or fread() error");
        message[n_read] = '\0';  // Null-terminate the string

        // Send the message
        if (socket_type == SOCK_STREAM) {
            if (write(sock, message, strlen(message)) == -1)
                error_handling("write() error");
            printf("TCP message sent: %s\n", message);
        } else {
            if (sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
                error_handling("sendto() error");
            printf("UDP message sent: %s\n", message);
        }

    } else if (strcmp(argv[4], "receive") == 0) {
        // Send the receive command to the server in TCP
        if (socket_type == SOCK_STREAM) {
            if (write(sock, "receive", strlen("receive")) == -1)
                error_handling("write() error");
        }
        
        // Receive the message
        if (socket_type == SOCK_STREAM) {
            str_len = read(sock, message, BUF_SIZE - 1);
            if (str_len == -1)
                error_handling("read() error");
        } else {
            socklen_t addr_size = sizeof(serv_addr);
            str_len = recvfrom(sock, message, BUF_SIZE - 1, 0, (struct sockaddr*)&serv_addr, &addr_size);
            if (str_len == -1)
                error_handling("recvfrom() error");
        }

        if (str_len > 0) {
            message[str_len] = '\0'; // Null-terminate the string
            printf("Message received: %s\n", message);
        } else {
            printf("Error: No messages received\n");
        }
    } else {
        error_handling("Invalid mode. Use 'send' or 'receive'.");
    }

    close(sock);
    return 0;
}

void error_handling(const char *message) {
    perror(message);
    exit(1);
}
