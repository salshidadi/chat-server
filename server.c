#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define MAX_HISTORY 100

typedef struct {
    int id;
    int fd;
    char name[32];
} user;

user users[MAX_CLIENTS];
int current_users = 0;

char history[MAX_HISTORY][512];
int history_count = 0;

int find_user_by_fd(int fd) {
    for (int i = 0; i < current_users; i++) {
        if (users[i].fd == fd) return i;
    }
    return -1;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    fd_set master_set, read_set;
    FD_ZERO(&master_set);
    FD_SET(server_fd, &master_set);
    int max_fd = server_fd;

    while (1) {
        read_set = master_set;

        if (select(max_fd + 1, &read_set, NULL, NULL, NULL) < 0) {
            break;
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_set)) {
                
                if (i == server_fd) {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

                    char name_buf[32];
                    memset(name_buf, 0, sizeof(name_buf));
                    int bytes = read(new_fd, name_buf, sizeof(name_buf) - 1);

                    if (bytes > 0) {
                        name_buf[strcspn(name_buf, "\n")] = 0;
                        
                        users[current_users].fd = new_fd;
                        strcpy(users[current_users].name, name_buf);
                        current_users++;

                        for (int h = 0; h < history_count; h++) {
                            write(new_fd, history[h], strlen(history[h]));
                        }

                        FD_SET(new_fd, &master_set);
                        if (new_fd > max_fd) {
                            max_fd = new_fd;
                        }
                    }
                } 
                else {
                    char msg_buf[256];
                    memset(msg_buf, 0, sizeof(msg_buf));
                    int bytes_read = read(i, msg_buf, sizeof(msg_buf) - 1);

                    int u_idx = find_user_by_fd(i);

                    if (bytes_read <= 0) {
                        if (u_idx != -1) {
                            users[u_idx].fd = -1;
                        }
                        close(i);
                        FD_CLR(i, &master_set);
                    } 
                    else {
                        if (u_idx != -1) {
                            msg_buf[strcspn(msg_buf, "\n")] = 0;
                            
                            char broadcast_msg[512];
                            snprintf(broadcast_msg, sizeof(broadcast_msg), "[%s]: %s\n", users[u_idx].name, msg_buf);

                            if (history_count < MAX_HISTORY) {
                                strcpy(history[history_count++], broadcast_msg);
                            }

                            for (int j = 0; j < current_users; j++) {
                                if (users[j].fd != -1) {
                                    write(users[j].fd, broadcast_msg, strlen(broadcast_msg));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}