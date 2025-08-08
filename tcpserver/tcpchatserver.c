#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../cvector/cvector.h"
#include <pthread.h>
#define PORT 8086
#define BUFFER_SIZE 1024
typedef enum{
    sendUsernameRequest,
    setUsername,
    chatting
}serverState;

void printUsername(char* username)
{
    for(int i = 0; i < strlen(username); i++)
    {
        printf("%c", username[i]);
    }
};

void printDataInt(int* val){
    printf("%d", *val);
};
void printDataChar(char* val){
    printf("%c", *val);
};
void printDataFloat(float* val){
    printf("%f", *val);
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
void parseChatMsg(vector *userMsgVector)
{
    for(int i = 0; i < userMsgVector->size; i++){
        void *data = malloc(userMsgVector->element_size);
        get_vector_element(userMsgVector, i, data);
        parseDataType(userMsgVector->type, data);
    }
};

void* handleclient(void* arg){
    int client_socket = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE] = {0};
    serverState currentState = sendUsernameRequest;
    char* username;
    while(1)
    {
        switch(currentState){
            case sendUsernameRequest:
                printf("Sending username request\n");
                char* sendUsernameRequest = "u";
                send(client_socket, sendUsernameRequest, sizeof(char), 0);
                currentState = setUsername;
                break;
            case setUsername:
                printf("Setting username from client\n");
                int bytes_recieved_username = recv(client_socket, buffer, BUFFER_SIZE, 0);
                if (bytes_recieved_username <= 0){
                    break;
                }
                username = strdup(buffer);
                currentState = chatting;
                memset(buffer, 0, BUFFER_SIZE);
                break;
            case chatting:
                int bytes_recieved_chat = recv(client_socket, buffer, BUFFER_SIZE, 0);
                    if (bytes_recieved_chat <= 0){
                    break;
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

                printf("Chatter from %s: ", username);
                parseChatMsg(&serverVector);
                printf("\n");
                sleep(1);
                memset(buffer, 0, BUFFER_SIZE);
                free(serverVector.data);
                break;
        
        }
    }

    printf("Shutdown command received");
    free(username);
    close(client_socket);
    return NULL;
};

int main() {
    int server_fd;
    struct sockaddr_in address;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server running\n");
    while (1)
    {
        if (listen(server_fd, 100) < 0) {
            perror("listen failed");
            break;
        }

        socklen_t addrlen = sizeof(address);
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (*client_sock < 0) {
            perror("accept failed");
            free(client_sock);
            continue;
        }

        pthread_t client_thread;
        if(pthread_create(&client_thread, NULL, handleclient, client_sock) != 0){
            perror("failed to create thread");
            close(*client_sock);
            free(client_sock);
        }

        pthread_detach(client_thread);
    }
    close(server_fd);
    return 0;
}