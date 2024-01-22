// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

struct Client {
    int socket;
    char username[50];
};

struct Client clients[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void* handle_client(void* arg);
void broadcast(char* message, int sender_socket);
void log_connection(char* username, char* action);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    pthread_t tid;

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Enlazar el socket a la dirección del servidor
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al enlazar el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error al escuchar conexiones");
        exit(EXIT_FAILURE);
    }

    printf("Servidor en espera de conexiones...\n");

    while (1) {
        // Aceptar una conexión entrante
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error al aceptar la conexión");
            continue;
        }

        // Crear un hilo para manejar al cliente
        pthread_create(&tid, NULL, handle_client, &client_socket);
    }

    close(server_socket);
    return 0;
}

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    char username[50];

    // Recibir el nombre de usuario del cliente
    if (recv(client_socket, username, sizeof(username), 0) <= 0) {
        perror("Error al recibir el nombre de usuario");
        close(client_socket);
        pthread_exit(NULL);
    }

    pthread_mutex_lock(&clients_mutex);

    // Agregar el cliente a la lista
    struct Client client;
    client.socket = client_socket;
    strcpy(client.username, username);
    clients[num_clients++] = client;

    // Enviar la lista de clientes a todos los clientes conectados
    char client_list[BUFFER_SIZE] = "Usuarios conectados: ";
    for (int i = 0; i < num_clients; ++i) {
        strcat(client_list, clients[i].username);
        strcat(client_list, " ");
    }
    broadcast(client_list, client_socket);

    pthread_mutex_unlock(&clients_mutex);

    // Notificar la conexión en el archivo de registro
    log_connection(username, "conectado");

    // Comunicación continua
    while (1) {
        if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
            // El cliente se desconectó
            pthread_mutex_lock(&clients_mutex);

            // Eliminar al cliente de la lista
            for (int i = 0; i < num_clients; ++i) {
                if (clients[i].socket == client_socket) {
                    log_connection(clients[i].username, "desconectado");
                    for (int j = i; j < num_clients - 1; ++j) {
                        clients[j] = clients[j + 1];
                    }
                    num_clients--;
                    break;
                }
            }

            pthread_mutex_unlock(&clients_mutex);

            // Enviar la lista actualizada a todos los clientes
            char client_list[BUFFER_SIZE] = "Usuarios conectados: ";
            for (int i = 0; i < num_clients; ++i) {
                strcat(client_list, clients[i].username);
                strcat(client_list, " ");
            }
            broadcast(client_list, client_socket);

            close(client_socket);
            pthread_exit(NULL);
        }

        // Broadcast del mensaje a todos los clientes
        broadcast(buffer, client_socket);
    }
}

void broadcast(char* message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < num_clients; ++i) {
        if (clients[i].socket != sender_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void log_connection(char* username, char* action) {
    FILE* log_file = fopen("log.txt", "a");

    if (log_file == NULL) {
        perror("Error al abrir el archivo de registro");
        return;
    }

    time_t now;
    time(&now);
    struct tm* tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(log_file, "[%s] Usuario %s %s\n", timestamp, username, action);

    fclose(log_file);
}
