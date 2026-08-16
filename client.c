#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        return 1;
    }

    printf("Enter your name: ");
    char my_name[32];
    if (fgets(my_name, sizeof(my_name), stdin) == NULL) return 1;
    my_name[strcspn(my_name, "\n")] = 0; 

    write(client_fd, my_name, strlen(my_name));

    fd_set read_set;
    int max_fd = client_fd;

    while(1) {
        FD_ZERO(&read_set);
        FD_SET(STDIN_FILENO, &read_set);
        FD_SET(client_fd, &read_set);

        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0) {
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &read_set)) {
            char msg_buffer[256];
            if (fgets(msg_buffer, sizeof(msg_buffer), stdin) == NULL) break;
            msg_buffer[strcspn(msg_buffer, "\n")] = 0; 

            write(client_fd, msg_buffer, strlen(msg_buffer));
        }

        if (FD_ISSET(client_fd, &read_set)) {
            char recv_buffer[512];
            memset(recv_buffer, 0, sizeof(recv_buffer));
            
            int bytes = read(client_fd, recv_buffer, sizeof(recv_buffer) - 1);
            if (bytes <= 0) {
                printf("\nServer disconnected.\n");
                break;
            }
            
            printf("%s", recv_buffer);
        }
    }

    close(client_fd);
    return 0;
}