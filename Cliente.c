// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP del servidor> <nombre de usuario>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* server_ip = argv[1];
    char* username = argv[2];

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket del cliente");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_aton(server_ip, &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar al servidor");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Enviar el nombre de usuario al servidor
    send(client_socket, username, strlen(username), 0);

    // Recibir mensajes del servidor
    char buffer[BUFFER_SIZE];
    while (1) {
        if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
            perror("Error al recibir mensajes del servidor");
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        printf("%s\n", buffer);
    }

    close(client_socket);
    return 0;
}
