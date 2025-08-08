#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "../cvector/cvector.h"

#define PORT 8086
#define BUFFER_SIZE 1024
#define VECTOR_SIZE 2

typedef enum{
    setUsername,
    sendMsg
}clientState;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *client_buffer = malloc(BUFFER_SIZE);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }
    char *client_name = malloc(BUFFER_SIZE);
    clientState currentState = setUsername;
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
    while(1){
        switch (currentState){
            case setUsername:
                printf("Set username:");
                int bytes_recieved_username = recv(sock, client_buffer, BUFFER_SIZE, 0);
                if (bytes_recieved_username <= 0){
                    break;
                }
                if (client_buffer[0] != 'u') {
                    printf("Unexpected message from server: %s\n", client_buffer);
                    sleep(1);
                    continue;
                }
                memset(client_buffer, 0, BUFFER_SIZE);
                fgets(client_buffer, BUFFER_SIZE, stdin);
                client_buffer[strcspn(client_buffer, "\n")] = '\0';
                send(sock, client_buffer, strlen(client_buffer), 0);
                currentState = sendMsg;
                printf("\n");
                client_name = memcpy(client_name, client_buffer, strlen(client_buffer));
                break;
            case sendMsg:
                vector msg;
                init_vector(&msg, sizeof(char), TYPE_CHAR);
                printf("%s says:", client_name);
                fgets(client_buffer, BUFFER_SIZE, stdin);
                printf("\n");
                client_buffer[strcspn(client_buffer, "\n")] = '\0';
                char *client_ptr = client_buffer;
                for(int i = 0; i < strlen(client_buffer); i++)
                {
                    push_back(&msg, &client_buffer[i]);
                }
                int total_data_size = msg.size * msg.element_size;
                int total_packet_size = 2 * sizeof(int) + sizeof(size_t) + sizeof(vectorDataTypes) + total_data_size;
                if (total_packet_size > BUFFER_SIZE) {
                    fprintf(stderr, "Error: total_packet_size (%d) exceeds BUFFER_SIZE (%d)\n", total_packet_size, BUFFER_SIZE);
                    break;
                }   
                memcpy(client_ptr, &msg.size, sizeof(int)); 
                client_ptr += sizeof(int);
                memcpy(client_ptr, &msg.capacity, sizeof(int)); 
                client_ptr += sizeof(int);
                memcpy(client_ptr, &msg.element_size, sizeof(size_t)); 
                client_ptr += sizeof(size_t);
                memcpy(client_ptr, &msg.type, sizeof(vectorDataTypes));
                client_ptr += sizeof(vectorDataTypes);
                memcpy(client_ptr, msg.data, total_data_size);

                send(sock, client_buffer, total_packet_size, 0);
                sleep(1);
                memset(client_buffer, 0, total_packet_size);
                break;
        }
    }
    free(client_name);
    free(client_buffer);
    close(sock);
    return 0;
}
