#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INTENTOS_MAX 10

char* palabras[] = {
    "linux",
    "semaforos",
    "profesor",
    "pongame",
    "diez",
    "porfas"
};

int obtenerIndiceAleatorio() {
    return rand() % (sizeof(palabras) / sizeof(palabras[0]));
}

int main() {
    srand(time(NULL));
    int indicePalabra = obtenerIndiceAleatorio();
    char* palabra = palabras[indicePalabra];

    int longitud = strlen(palabra);
    char palabraAdivinada[longitud];
    memset(palabraAdivinada, '_', longitud);
    palabraAdivinada[longitud] = '\0';

    int intentosRestantes = INTENTOS_MAX;
    char letrasUsadas[26];
    memset(letrasUsadas, 0, 26);

    printf("¡Bienvenido al Juego del Ahorcado!\n");

    while (intentosRestantes > 0) {
        printf("Palabra a adivinar: %s\n", palabraAdivinada);
        printf("Letras usadas: ");
        for (int i = 0; i < 26; i++) {
            if (letrasUsadas[i]) {
                printf("%c ", 'a' + i);
            }
        }
        printf("\nIntentos restantes: %d\n", intentosRestantes);

        char letra;
        printf("Ingresa una letra: ");
        scanf(" %c", &letra);

        if (letrasUsadas[letra - 'a']) {
            printf("Ya has utilizado esa letra. Intenta con otra.\n");
            continue;
        }

        letrasUsadas[letra - 'a'] = 1;

        int aciertos = 0;
        for (int i = 0; i < longitud; i++) {
            if (palabra[i] == letra) {
                palabraAdivinada[i] = letra;
                aciertos = 1;
            }
        }

        if (!aciertos) {
            intentosRestantes--;
            printf("Letra '%c' no está en la palabra. ¡Te quedan %d intentos!\n", letra, intentosRestantes);
        }

        if (strcmp(palabraAdivinada, palabra) == 0) {
            printf("¡Felicidades! Has adivinado la palabra: %s\n", palabra);
            break;
        }
    }

    if (strcmp(palabraAdivinada, palabra) != 0) {
        printf("¡Perdiste! La palabra era: %s\n", palabra);
    }

    return 0;
}
