#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../cvector/cvector.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define VECTOR_SIZE 2
int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *client_buffer = malloc(BUFFER_SIZE);
    vector *vectorContainer[2];
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

    vector *charVector = malloc(sizeof(vector));
    init_vector(charVector, sizeof(char), TYPE_CHAR);

    vector *intVector = malloc(sizeof(vector));
    init_vector(intVector, sizeof(int), TYPE_INT);

    vectorContainer[0] = charVector;
    vectorContainer[1] = intVector;

    for(int i = 0; i < 20; i++){
        push_back(vectorContainer[1],&i);
    }

    char *name = "emil";
    for (int i = 0; i < strlen(name); i++) {
        push_back(vectorContainer[0], &name[i]);
    }  
    for(int i = 0; i < VECTOR_SIZE; i++)
    {
        int total_data_size = vectorContainer[i]->size * vectorContainer[i]->element_size;
        int total_packet_size = 2 * sizeof(int) + sizeof(size_t) + sizeof(vectorDataTypes) + total_data_size;
        char *temp_ptr = realloc(client_buffer, total_packet_size);
        if (!temp_ptr) {
            perror("Realloc failed");
            free(client_buffer);
            return -1;
        }
        client_buffer = temp_ptr;
        char *client_ptr = client_buffer;

        memcpy(client_ptr, &vectorContainer[i]->size, sizeof(int)); 
        client_ptr += sizeof(int);
        memcpy(client_ptr, &vectorContainer[i]->capacity, sizeof(int)); 
        client_ptr += sizeof(int);
        memcpy(client_ptr, &vectorContainer[i]->element_size, sizeof(size_t)); 
        client_ptr += sizeof(size_t);
        memcpy(client_ptr, &vectorContainer[i]->type, sizeof(vectorDataTypes));
        client_ptr += sizeof(vectorDataTypes);
        memcpy(client_ptr, vectorContainer[i]->data, total_data_size);

        send(sock, client_buffer, total_packet_size, 0);
        printf("Message sent to server.\n");
        sleep(1);
        memset(client_buffer, 0, total_packet_size);
    }
    free(client_buffer);
    close(sock);
    return 0;
}
