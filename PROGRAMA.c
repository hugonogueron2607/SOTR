#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

#define BOARD_SIZE 5

pthread_mutex_t mutex;  // Mutex para la exclusión mutua en la sección crítica

// Estructura que representa un jugador
struct Player {
    char board[BOARD_SIZE][BOARD_SIZE];
    bool enemy_ships[BOARD_SIZE][BOARD_SIZE];
    int player_number;
};

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

// Función para colocar los barcos en el tablero
void place_ships(struct Player *player) {
    int num_ships_placed = 0;
    int num_ships;

    printf("Jugador %d, ingresa cuantos barcos deseas colocar\n", player->player_number);
    scanf("%d", &num_ships);

    while (num_ships_placed < num_ships) {
        char x_char;
        int y;
        printf("\nJugador %d, coloca el barco %d\n", player->player_number, num_ships_placed + 1);
        printf("Ingresa la coordenada (por ejemplo, A5): ");
        scanf(" %c%d", &x_char, &y);

        int x = toupper(x_char) - 'A'; // Convertir letra a índice
        y--;                           // Ajustar el índice a partir de 0

        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE &&
            player->board[y][x] == '~') {
            player->board[y][x] = 'S';
            num_ships_placed++;
            player->enemy_ships[y][x] = true;
            printf("Barco colocado en la posición (%c, %d)\n", 'A' + x, y + 1);
        } else {
            printf("Posición inválida. Inténtalo de nuevo.\n");
        }
    }
}

// Función para jugar el juego
void play_game(struct Player *current_player, struct Player *other_player) {
    printf("\nJugador %d, es tu turno!\n", current_player->player_number);
    printf("Tu tablero:\n");
    print_board(current_player->board);

    char x_char;
    int y;
    printf("Ingresa la coordenada a disparar (por ejemplo, A5): ");
    scanf(" %c%d", &x_char, &y);

    int x = toupper(x_char) - 'A'; // Convertir letra a índice
    y--;                           // Ajustar el índice a partir de 0

    pthread_mutex_lock(&mutex); // Bloquear mutex antes de entrar en la sección crítica

    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (other_player->enemy_ships[y][x]) {
            printf("¡Impacto! Has alcanzado un barco enemigo en la posición (%c, %d)!\n",
                   'A' + x, y + 1);
            current_player->board[y][x] = 'X'; // Marcamos el impacto en el tablero del jugador
            other_player->enemy_ships[y][x] = false; // "Hundimos" el barco enemigo
        } else {
            printf("Disparo al agua en la posición (%c, %d).\n", 'A' + x, y + 1);
            current_player->board[y][x] = 'O'; // Marcamos el disparo al agua en el tablero del jugador
        }
    } else {
        printf("Disparo fuera de los límites. Inténtalo de nuevo.\n");
    }

    pthread_mutex_unlock(&mutex); // Desbloquear mutex después de salir de la sección crítica
}

// Funcion para terminar el juego
bool is_game_over(struct Player *player1, struct Player *player2) {
    int remaining_ships_player_1 = 0;
    int remaining_ships_player_2 = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (player1->enemy_ships[i][j]) {
                remaining_ships_player_1++;
            }
            if (player2->enemy_ships[i][j]) {
                remaining_ships_player_2++;
            }
        }
    }
    printf("Al Jugador 1 le falta tirar %d\n", remaining_ships_player_1);
    printf("Al Jugador 2 le falta tirar %d\n", remaining_ships_player_2);

    return (remaining_ships_player_1 == 0 || remaining_ships_player_2 == 0);
}

// Funcion para determinar el ganador
int winner(struct Player *player1, struct Player *player2) {
    int remaining_ships_player_1 = 0;
    int remaining_ships_player_2 = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (player1->enemy_ships[i][j]) {
                remaining_ships_player_1++;
            }
            if (player2->enemy_ships[i][j]) {
                remaining_ships_player_2++;
            }
        }
    }
    if (remaining_ships_player_1 == 0 && remaining_ships_player_2 > 0) {
        return 2; // Jugador 2 gana
    } else if (remaining_ships_player_2 == 0 && remaining_ships_player_1 > 0) {
        return 1; // Jugador 1 gana
    }
    return 0; // El juego aún no ha terminado
}


// Función para el hilo de un jugador
void *player_thread(void *arg) {
    struct Player *current_player = (struct Player *)arg;

    place_ships(current_player); // Colocar los barcos en el tablero

    while (!is_game_over(current_player, (struct Player *)arg)) {
        play_game(current_player, (struct Player *)arg); // Jugar el juego
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t player1_thread, player2_thread;

    struct Player player1, player2;

    // Inicializar los tableros y estructuras de jugador
    init_board(player1.board);
    init_board(player2.board);
    memset(player1.enemy_ships, false, sizeof(player1.enemy_ships));
    memset(player2.enemy_ships, false, sizeof(player2.enemy_ships));
    player1.player_number = 1;
    player2.player_number = 2;

    // Inicializar el mutex
    pthread_mutex_init(&mutex, NULL);

    // Crear hilos para cada jugador
    pthread_create(&player1_thread, NULL, player_thread, (void *)&player1);
    pthread_create(&player2_thread, NULL, player_thread, (void *)&player2);

    // Esperar a que ambos hilos terminen
    pthread_join(player1_thread, NULL);
    pthread_join(player2_thread, NULL);

    // Destruir el mutex
    pthread_mutex_destroy(&mutex);

    // Determinar al ganador
    int result = winner(&player1, &player2);

    // Imprimir el resultado
    if (result == 1) {
        printf("¡Jugador 1 es el ganador!\n");
    } else if (result == 2) {
        printf("¡Jugador 2 es el ganador!\n");
    } else {
        printf("El juego terminó en empate.\n");
    }

    return 0;
}
