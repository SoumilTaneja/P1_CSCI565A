#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 3000
#define MAX_QUEUE 100

void error_handling(const char *message);
void *client_handler(void *arg);
void enqueue_message(const char *message);
char *dequeue_message();

pthread_mutex_t mutex;
char *message_queue[MAX_QUEUE];
int queue_start = 0;
int queue_end = 0;

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    pthread_t t_id;
    int socket_type;

    // Validate the number of arguments
    if (argc != 3) {
        printf("Usage: %s <port> <TCP|UDP>\n", argv[0]);
        exit(1);
    }

    // Determine socket type
    if (strcmp(argv[2], "TCP") == 0) {
        socket_type = SOCK_STREAM;
    } else if (strcmp(argv[2], "UDP") == 0) {
        socket_type = SOCK_DGRAM;
    } else {
        error_handling("Invalid socket type. Use 'TCP' or 'UDP'.");
    }

    pthread_mutex_init(&mutex, NULL);

    // Create socket
    serv_sock = socket(PF_INET, socket_type, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    // Configure server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Bind to any available network interface
    serv_addr.sin_port = htons(atoi(argv[1]));

    // Bind socket
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind() error");
        close(serv_sock);
        exit(1);
    }

    if (socket_type == SOCK_STREAM) {
        // Listen for connections
        if (listen(serv_sock, 5) == -1) {
            perror("listen() error");
            close(serv_sock);
            exit(1);
        }

        while (1) {
            clnt_addr_size = sizeof(clnt_addr);
            clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
            if (clnt_sock == -1) {
                perror("accept() error");
                continue;  // Continue to accept other connections even if this one fails
            }

            // Create a thread to handle the client
            if (pthread_create(&t_id, NULL, client_handler, (void*)&clnt_sock) != 0) {
                perror("pthread_create() error");
                close(clnt_sock);
                continue;
            }
            pthread_detach(t_id);  // Detach thread to avoid memory leak
        }
    } else {
        // Handle UDP connections
        char message[BUF_SIZE];
        while (1) {
            clnt_addr_size = sizeof(clnt_addr);
            int str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
            if (str_len > 0) {
                message[str_len] = 0;
                enqueue_message(message);
                printf("Received UDP message: %s\n", message);
            } else {
                perror("recvfrom() error");
            }
        }
    }

    close(serv_sock);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void *client_handler(void *arg) {
    int clnt_sock = *((int*)arg);
    char message[BUF_SIZE];
    int str_len;

    // Read message from client
    str_len = read(clnt_sock, message, sizeof(message) - 1);
    if (str_len > 0) {
        message[str_len] = 0;
        // Check if the client is sending or receiving a message
        if (strcmp(message, "receive") == 0) {
            char *msg_to_send = dequeue_message();
            if (msg_to_send != NULL) {
                write(clnt_sock, msg_to_send, strlen(msg_to_send));
                printf("Sent TCP response: %s\n", msg_to_send);
                free(msg_to_send);
            } else {
                const char *no_msg = "Error: No messages\n";
                write(clnt_sock, no_msg, strlen(no_msg));
            }
        } else {
            enqueue_message(message);
            printf("Received TCP message: %s\n", message);
        }
    } else {
        perror("read() error");
    }

    close(clnt_sock);
    return NULL;
}

void enqueue_message(const char *message) {
    pthread_mutex_lock(&mutex);

    if ((queue_end + 1) % MAX_QUEUE == queue_start) {
        printf("Queue is full! Cannot enqueue message.\n");
    } else {
        message_queue[queue_end] = strdup(message);
        queue_end = (queue_end + 1) % MAX_QUEUE;
    }

    pthread_mutex_unlock(&mutex);
}

char *dequeue_message() {
    pthread_mutex_lock(&mutex);

    char *message = NULL;
    if (queue_start == queue_end) {
        printf("Queue is empty!\n");
    } else {
        message = message_queue[queue_start];
        queue_start = (queue_start + 1) % MAX_QUEUE;
    }

    pthread_mutex_unlock(&mutex);
    return message;
}

void error_handling(const char *message) {
    perror(message);
    exit(1);
}
