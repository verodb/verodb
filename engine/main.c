#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "storage.h"
#include "query.h"
#include "aof.h"

#define PORT 54321
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

Database globalDb = {0};
pthread_mutex_t dbMutex = PTHREAD_MUTEX_INITIALIZER;

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0'; 
        printf("Received query: %s\n", buffer);
        char originalQuery[BUFFER_SIZE];
        strncpy(originalQuery, buffer, sizeof(originalQuery) - 1);

        Query* query = parseCreateDatabaseQuery(buffer);

        if (query) {
            pthread_mutex_lock(&dbMutex);

            if (query->type == CREATE_DATABASE) {
                if (strlen(globalDb.name) == 0) {
                    executeCreateDatabaseQuery(&globalDb, query);
                    snprintf(buffer, sizeof(buffer), "Database %s created successfully.", query->databaseName);
                    appendToAof(originalQuery);  // Log the successful operation
                } else {
                    snprintf(buffer, sizeof(buffer), "Error: Database already exists.");
                    printf("Database already exists.\n");
                }
            } else {
                snprintf(buffer, sizeof(buffer), "Error: Unsupported query type.");
                printf("Unsupported query type.\n");
            }

            pthread_mutex_unlock(&dbMutex);
            free(query);
        } else {
            snprintf(buffer, sizeof(buffer), "Error: Failed to parse query.");
            printf("Failed to parse query: %s\n", buffer);
        }

        if (send(clientSocket, buffer, strlen(buffer), 0) == -1) {
            perror("send");
        }
    }

    if (bytesRead < 0) {
        perror("recv");
    } else if (bytesRead == 0) {
        printf("Client disconnected.\n");
    }

    close(clientSocket);
}

void replayAofLog(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        executeCommand(buffer);
    }

    fclose(file);
}

void executeCommand(const char *command) {
    Query *query = parseCreateDatabaseQuery(command);
    if (query) {
        if (query->type == CREATE_DATABASE) {
            if (strlen(globalDb.name) == 0) {
                executeCreateDatabaseQuery(&globalDb, query);
            }
        }
        free(query);
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("listen");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    initAof("aof.log");  // Initialize AOF logging
    replayAofLog("aof.log");  // Replay AOF logs

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            perror("accept");
            continue;
        }

        printf("Accepted connection from %s:%d\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        pthread_t thread;
        if (pthread_create(&thread, NULL, (void* (*)(void*))handleClient, (void*)(intptr_t)clientSocket) != 0) {
            perror("pthread_create");
            close(clientSocket);
        } else {
            pthread_detach(thread);
        }
    }

    close(serverSocket);
    pthread_mutex_destroy(&dbMutex);
    closeAof();  // Close AOF file before exiting
    return 0;
}
