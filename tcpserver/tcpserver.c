#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    int sockets;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server running! '\n'");
    while (1)
    {
        if (listen(server_fd, 3) < 0) {
            perror("listen failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        socklen_t addrlen = sizeof(address);
        sockets = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (sockets < 0) {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        while(buffer[0] != 'Q')
        {
            memset(buffer, 0, BUFFER_SIZE);
            read(sockets, buffer, BUFFER_SIZE);
            printf("Received from client: %s\n", buffer);
            sleep(1);
        }

    }
    printf("Shutdown command received");
    close(sockets);
    close(server_fd);
    return 0;
}
