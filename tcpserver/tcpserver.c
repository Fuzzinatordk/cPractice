#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../cvector/cvector.h"
#define PORT 8080
#define BUFFER_SIZE 1024

void printDataInt(int* val){
    printf("Value: %d\n", *val);
};
void printDataChar(char* val){
    printf("Value: %c\n", *val);
};
void printDataFloat(float* val){
    printf("Value: %f\n", *val);
};
void parseDataType(vectorDataTypes type, void* data) {
    switch(type) {
        case TYPE_CHAR:
            printDataChar((char*)data);
            break;
        case TYPE_INT:
            printDataInt((int*)data);
            break;
        case TYPE_FLOAT:
            printDataFloat((float*)data);
            break;
        default:
            printf("Unknown type\n");
            break;
    }
}

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
    printf("Server running\n");
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
                sleep(1);
                continue;

            }
            vector serverVector;
            char *server_ptr = buffer;

            memcpy(&serverVector.size, server_ptr, sizeof(int));
            server_ptr += sizeof(int);
            memcpy(&serverVector.capacity, server_ptr, sizeof(int));
            server_ptr += sizeof(int);
            memcpy(&serverVector.element_size, server_ptr, sizeof(size_t));
            server_ptr += sizeof(size_t);
            memcpy(&serverVector.type, server_ptr, sizeof(vectorDataTypes));
            server_ptr += sizeof(vectorDataTypes);

            int total_recieved_data_size = serverVector.size * serverVector.element_size;
            serverVector.data = malloc(total_recieved_data_size);
            memcpy(serverVector.data, server_ptr, total_recieved_data_size);
            printf("Received a vector from client. Size: %d\n", serverVector.size);
            printf("The vector contains the following:\n");
            for(int i = 0; i < serverVector.size; i++){
                void *data;
                get_vector_element(&serverVector, i, data);
                parseDataType(serverVector.type, data);
            }
            sleep(1);
        }

    }
    printf("Shutdown command received");
    close(sockets);
    close(server_fd);
    return 0;
}
