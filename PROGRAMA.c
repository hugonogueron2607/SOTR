#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BOARD_SIZE 5

// Estructura para representar el tablero
typedef struct {
    char board[BOARD_SIZE][BOARD_SIZE];
    pthread_mutex_t mutex;
} GameBoard;

GameBoard gameBoard;

// Función para inicializar el tablero
void initializeBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            gameBoard.board[i][j] = '-';
        }
    }
}

// Función para imprimir el tablero
void printBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", gameBoard.board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Función para realizar la jugada de un jugador
void* playerMove(void* player) {
    int* playerId = (int*)player;

    while (1) {
        pthread_mutex_lock(&gameBoard.mutex);

        printf("Jugador %d, es tu turno.\n", *playerId);
        int x, y;
        printf("Introduce las coordenadas (fila columna): ");
        scanf("%d %d", &x, &y);

        if (gameBoard.board[x][y] == '-') {
            gameBoard.board[x][y] = *playerId + '0';
            printf("¡Acertaste!\n");
            printBoard();
        } else {
            printf("¡Agua!\n");
        }

        pthread_mutex_unlock(&gameBoard.mutex);
        sleep(1);
    }

    return NULL;
}

int main() {
    initializeBoard();
    printBoard();

    pthread_t player1, player2;
    int playerId1 = 1, playerId2 = 2;

    pthread_mutex_init(&gameBoard.mutex, NULL);

    pthread_create(&player1, NULL, playerMove, &playerId1);
    pthread_create(&player2, NULL, playerMove, &playerId2);

    pthread_join(player1, NULL);
    pthread_join(player2, NULL);

    pthread_mutex_destroy(&gameBoard.mutex);

    return 0;
}