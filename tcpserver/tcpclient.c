#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../cvector/cvector.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *client_buffer = malloc(BUFFER_SIZE);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    while(client_buffer[0] != 'Q')
    {
        client_buffer = memset(client_buffer, 0, BUFFER_SIZE);
        vector clientVector;
        init_vector(&clientVector, sizeof(int));
        for(int i = 0; i < 20; i++){
            push_back(&clientVector,&i);
        }
        int total_data_size = clientVector.size * clientVector.element_size;
        int total_packet_size = 2 * sizeof(int) + sizeof(size_t) + total_data_size;
        char *temp_ptr = realloc(client_buffer,total_packet_size);
        client_buffer = temp_ptr;
        char *client_ptr = client_buffer;

        memcpy(client_ptr, &clientVector.size, sizeof(int)); client_ptr += sizeof(int);
        memcpy(client_ptr, &clientVector.capacity, sizeof(int)); client_ptr += sizeof(int);
        memcpy(client_ptr, &clientVector.element_size, sizeof(size_t)); client_ptr += sizeof(size_t);

        memcpy(client_ptr, clientVector.data, total_data_size);

        send(sock, client_buffer, strlen(client_buffer), 0);
        printf("Message sent to server.\n");
        break;
    }
    free(client_buffer);
    close(sock);
    return 0;
}
