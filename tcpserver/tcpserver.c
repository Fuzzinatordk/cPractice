#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../cvector/cvector.h"

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
    printf("Server running");
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
            int bytes_recieved = recv(sockets, buffer, BUFFER_SIZE, 0);
            if (bytes_recieved <= 0){
                continue;
                sleep(1);
            }
            vector serverVector;
            char *server_ptr = buffer;

            memcpy(&serverVector.size, server_ptr, sizeof(int));
            server_ptr += sizeof(int);
            memcpy(&serverVector.capacity, server_ptr, sizeof(int));
            server_ptr += sizeof(int);
            memcpy(&serverVector.element_size, server_ptr, sizeof(size_t));
            server_ptr += sizeof(size_t);

            int total_recieved_data_size = serverVector.size * serverVector.element_size;
            serverVector.data = malloc(total_recieved_data_size);
            memcpy(serverVector.data, server_ptr, total_recieved_data_size);

            printf("Received a vector from client. with size %u '\n'", serverVector.size);
            sleep(1);
        }

    }
    printf("Shutdown command received");
    close(sockets);
    close(server_fd);
    return 0;
}
