#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_CONNECTIONS 2
#define BOARD_SIZE 5

// Función para inicializar el tablero con agua ('~')
void init_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '~';
        }
    }
}

// Función para imprimir el tablero
void print_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("   ");
        for (char c = 'A'; c < 'A' + BOARD_SIZE; c++) {
            printf("%c ", c);
        }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

// Funcion para terminar el juego 
bool is_game_over(bool enemy_ships_player_1[BOARD_SIZE][BOARD_SIZE], bool enemy_ships_player_2[BOARD_SIZE][BOARD_SIZE]) {
    int remaining_ships_player_1 = 0;
    int remaining_ships_player_2 = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (enemy_ships_player_1[i][j]) {
                remaining_ships_player_1++;
            }
            if (enemy_ships_player_2[i][j]) {
                remaining_ships_player_2++;
            }
        }
    }
    printf("Al jugador 1 le falta tirar %d\n", remaining_ships_player_1);
    printf("Al jugador 2 le falta tirar %d\n", remaining_ships_player_2);

    return (remaining_ships_player_1 == 0 || remaining_ships_player_2 == 0) ;
}

// Funcion para determinar el ganador
int winner(bool enemy_ships_player_1[BOARD_SIZE][BOARD_SIZE], bool enemy_ships_player_2[BOARD_SIZE][BOARD_SIZE]) {
    int remaining_ships_player_1 = 0;
    int remaining_ships_player_2 = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (enemy_ships_player_1[i][j]) {
                remaining_ships_player_1++;
            }
            if (enemy_ships_player_2[i][j]) {
                remaining_ships_player_2++;
            }
        }
    }
    if (remaining_ships_player_1 == 0 && remaining_ships_player_2 > 0) {
        return 2;
    } else if (remaining_ships_player_2 == 0 && remaining_ships_player_1 > 0) {
        return 1;
    }
}


int main() {
    //Sockets
    int server_socket, client_socket[MAX_CONNECTIONS];
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;
    char chat[1024];

    //Battleships
    char player_board_1[BOARD_SIZE][BOARD_SIZE];
    char player_board_2[BOARD_SIZE][BOARD_SIZE];
    char game_board_1[BOARD_SIZE][BOARD_SIZE];
    char game_board_2[BOARD_SIZE][BOARD_SIZE];
    int y, x; 

    // Matriz para registrar la ubicación de los barcos enemigos
    bool enemy_ships_player_1[BOARD_SIZE][BOARD_SIZE] = {false};
    bool enemy_ships_player_2[BOARD_SIZE][BOARD_SIZE] = {false};  

    // Crear el socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Vincular el socket a la dirección y al puerto
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error al vincular el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        perror("Error al escuchar conexiones");
        exit(EXIT_FAILURE);
    }

    printf("Esperando a los jugadores...\n");

    // Aceptar conexiones de dos clientes
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        client_address_len = sizeof(client_address);
        client_socket[i] = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket[i] == -1) {
            perror("Error al aceptar la conexión");
            exit(EXIT_FAILURE);
        }
        printf("Jugador %d conectado\n", i + 1);

        // Enviar mensaje de bienvenida al cliente
        send(client_socket[i], "Bienvenido", sizeof(chat), 0);
        send(client_socket[i], "Espera a tu oponente", sizeof(chat), 0);
    }

    //Empieza el juego 
    init_board(player_board_1);
    init_board(player_board_2);
    init_board(game_board_1);
    init_board(game_board_2);

    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        send(client_socket[i], "Comienza el juego", sizeof(chat), 0);
        send(client_socket[i], "Coloca tus barcos", sizeof(chat), 0);
        if ( i == 0 ){
            send(client_socket[i], player_board_1, sizeof(player_board_1), 0);
            recv(client_socket[i], player_board_1, sizeof(player_board_1), 0);
            recv(client_socket[i], enemy_ships_player_1, sizeof(enemy_ships_player_1), 0);
        } else {
            send(client_socket[i], player_board_2, sizeof(player_board_2), 0);
            recv(client_socket[i], player_board_2, sizeof(player_board_2), 0);
            recv(client_socket[i], enemy_ships_player_2, sizeof(enemy_ships_player_2), 0);
        }
    }

    // Revisar que esté bien la información
    print_board(player_board_1);
    print_board(player_board_2);

    int turno = 1;
    int intentos = 1;
    while (!is_game_over(enemy_ships_player_1, enemy_ships_player_2)){
        sprintf(chat, "Turno No. %d", intentos);
        printf("%s\n", chat);
        if (turno == 1 ){
            send(client_socket[0], chat, sizeof(chat), 0);

            send(client_socket[0], game_board_1, sizeof(game_board_1), 0);
            send(client_socket[0], enemy_ships_player_2, sizeof(enemy_ships_player_2), 0);

            recv(client_socket[0], game_board_1, sizeof(game_board_1), 0);
            recv(client_socket[0], enemy_ships_player_2, sizeof(enemy_ships_player_2), 0);
            
            printf("Tablero Jugador 1\n");
            print_board(game_board_1);
        } else {
            send(client_socket[1], chat, sizeof(chat), 0);

            send(client_socket[1], game_board_2, sizeof(game_board_2), 0);
            send(client_socket[1], enemy_ships_player_1, sizeof(enemy_ships_player_1), 0);

            recv(client_socket[1], game_board_2, sizeof(game_board_2), 0);
            recv(client_socket[1], enemy_ships_player_1, sizeof(enemy_ships_player_1), 0);
            
            printf("Tablero Jugador 2\n");
            print_board(game_board_2);
            
            intentos++;
        }
        turno = (turno == 1) ? 2 : 1;
    }

    if (winner(enemy_ships_player_1, enemy_ships_player_2) == 1) {
        printf("Ganó el jugador 1\n");
        send(client_socket[0], "Ganaste", sizeof(chat), 0);
        send(client_socket[1], "Perdiste", sizeof(chat), 0);
    } else if (winner(enemy_ships_player_1, enemy_ships_player_2) == 2){
        printf("Ganó el jugador 2\n");
        send(client_socket[0], "Perdiste", sizeof(chat), 0);
        send(client_socket[1], "Ganaste", sizeof(chat), 0);
    }

    // Cerrar conexiones
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        close(client_socket[i]);
    }

    close(server_socket);

    return 0;
}