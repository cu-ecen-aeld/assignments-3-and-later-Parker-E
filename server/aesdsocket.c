#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 9000
#define FILE_PATH "/var/tmp/aesdsocketdata"
#define BUFFER_SIZE 1024

int running = 1;
int sock_fd = -1;
int accept_fd = -1;

void signal_handler(int signum) {
    running = 0;
    if (signum == SIGINT || signum == SIGTERM)
    remove("/var/tmp/aesdsocketdata");
    syslog(LOG_INFO, "Caught signal, exiting");
    if (sock_fd >= 0) close(sock_fd);
    if (accept_fd >= 0) close(accept_fd);
}


void daemonize() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("First fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("Second fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
        exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[]) {
    int daemon_mode = 0;

    if (argc == 2 && strcmp(argv[1], "-d") == 0)
        daemon_mode = 1;

    struct sockaddr_in serv_addr = {0}, client_addr;
    socklen_t client_len = sizeof(client_addr);

    openlog("aesdsocket", LOG_PID, LOG_USER);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0) {
        perror("socket");
        return -1;
    }

    int yes = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sock_fd);
        return -1;
    }

    if (daemon_mode == 1)
        daemonize();

    if (listen(sock_fd, 10) < 0) {
        perror("listen");
        close(sock_fd);
        return -1;
    }


    while (running) {
        accept_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_len);
        if (accept_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            break;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        syslog(LOG_DEBUG, "Accepted connection from %s", client_ip);

        char* recv_buffer = NULL;
        size_t recv_size = 0;
        ssize_t received;
        char temp_buffer[BUFFER_SIZE];

        int packet_complete = 0;
        while (!packet_complete && (received = recv(accept_fd, temp_buffer, sizeof(temp_buffer), 0)) > 0) {
            char* new_buf = realloc(recv_buffer, recv_size + received + 1);
            if (!new_buf) {
                perror("realloc");
                free(recv_buffer);
                break;
            }

            recv_buffer = new_buf;
            memcpy(recv_buffer + recv_size, temp_buffer, received);
            recv_size += received;
            recv_buffer[recv_size] = '\0';

            if (strchr(recv_buffer, '\n')) {
                packet_complete = 1;
            }
        }

        if (packet_complete && recv_buffer != NULL) {
            FILE* file = fopen(FILE_PATH, "a");
            if (file) {
                fwrite(recv_buffer, sizeof(char), recv_size, file);
                fclose(file);
            }

            file = fopen(FILE_PATH, "r");
            if (file) {
                char send_buf[BUFFER_SIZE];
                size_t read_bytes;
                while ((read_bytes = fread(send_buf, 1, BUFFER_SIZE, file)) > 0) {
                    send(accept_fd, send_buf, read_bytes, 0);
                }
                fclose(file);
            }
        }

        free(recv_buffer);

        syslog(LOG_DEBUG, "Closed connection from %s", client_ip);
        close(accept_fd);
    }

    close(sock_fd);
    remove(FILE_PATH);
    syslog(LOG_INFO, "Server exiting");
    closelog();

    return 0;
}
