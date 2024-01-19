#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

#define BOARD_SIZE 5

// Estructura para representar el tablero
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    pthread_mutex_t mutex;
} GameBoard;

GameBoard gameBoard;

    char player_board[BOARD_SIZE][BOARD_SIZE];
    char player_board_1[BOARD_SIZE][BOARD_SIZE];
    char player_board_2[BOARD_SIZE][BOARD_SIZE];
    char game_board[BOARD_SIZE][BOARD_SIZE];
    char game_board_1[BOARD_SIZE][BOARD_SIZE];
    char game_board_2[BOARD_SIZE][BOARD_SIZE];
    bool enemy_ships[BOARD_SIZE][BOARD_SIZE]={false};
    bool enemy_ships_player_1[BOARD_SIZE][BOARD_SIZE]={false};
    bool enemy_ships_player_2[BOARD_SIZE][BOARD_SIZE]={false};

// Función para inicializar el tablero
void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            gameBoard.board[i][j] = '~';
        }
    }
}

// Función para imprimir el tablero
void printBoard() {
    printf("   ");
        for (char c = 'A'; c < 'A' + BOARD_SIZE; c++) {
            printf("%c ", c);
        }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", gameBoard.board[i][j]);
        }
        printf("\n");
    }
}
void* place_ships(void* player) {
    int* playerId = (int*)player;
    char board[BOARD_SIZE][BOARD_SIZE];
    bool enemy_ships[BOARD_SIZE][BOARD_SIZE]; 
    int num_ships_placed = 0;
    int num_ships;
    
    printf("Ingresa cuantos barcos deseas colocar\n");
    scanf("%d", &num_ships);

    while (num_ships_placed < num_ships) {
        char x_char;
        int y;
        printf("\nColoca el barco %d\n", num_ships_placed + 1);
        printf("Ingresa la coordenada (por ejemplo, A5): ");
        scanf(" %c%d", &x_char, &y);

        int x = toupper(x_char) - 'A'; // Convertir letra a índice
        y--;                           // Ajustar el índice a partir de 0

        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE &&
            board[y][x] == '~') {
            board[y][x] = 'S';
            num_ships_placed++;
            enemy_ships[y][x] = true;
            printf("Barco colocado en la posición (%c, %d)\n", 'A' + x, y + 1);
        } else {
            printf("Posición inválida. Inténtalo de nuevo.\n");
        }
    }
}


// Función para realizar la jugada de un jugador
void* playerMove(void* player) {
    int* playerId = (int*)player;
    char player_board[BOARD_SIZE][BOARD_SIZE];
    bool enemy_ships[BOARD_SIZE][BOARD_SIZE];
    while (1) {
        pthread_mutex_lock(&gameBoard.mutex);
        
        char x_char;
        int y;
        printf("Jugador %d, es tu turno.\n", *playerId);        
        printf("Introduce las coordenadas (ejemplo, A5): ");
        scanf("%c %d", &x_char, &y);

    int x = toupper(x_char) - 'A'; // Convertir letra a índice
    y--;                           // Ajustar el índice a partir de 0

    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (enemy_ships[y][x]) {
            printf("¡Impacto! Has alcanzado un barco enemigo en la posición (%c, "
                "%d)!\n",
                'A' + x, y + 1);
            player_board[y][x] =
                'X'; // Marcamos el impacto en el tablero del jugador
            enemy_ships[y][x] = false; // "Hundimos" el barco enemigo
        } else {
            printf("Disparo al agua en la posición (%c, %d).\n", 'A' + x, y + 1);
            player_board[y][x] =
                'O'; // Marcamos el disparo al agua en el tablero del jugador
        }
    } else {
        printf("Disparo fuera de los límites. Inténtalo de nuevo.\n");
    }
}
}

int main() {
    init_board(player_board_1);
    init_board(player_board_2);
    init_board(game_board_1);
    init_board(game_board_2);
    
    printf("Bienvendio a Batalla Naval:\n");

    pthread_t player1, player2;
    int playerId1 = 1, playerId2 = 2;

    pthread_mutex_init(&gameBoard.mutex, NULL);

    pthread_create(&player1, NULL, place_ships, &playerId1);
    pthread_create(&player2, NULL, place_ships, &playerId2);
    //pthread_create(&player1, NULL, playerMove, &playerId1);
    //pthread_create(&player2, NULL, playerMove, &playerId2);

    pthread_join(player1, NULL);
    pthread_join(player2, NULL);

    pthread_mutex_destroy(&gameBoard.mutex);

    return 0;
}