#include <stdio.h>
#include <pthread.h>


int shared_variable = 0;

void *increment_thread(void *arg) {
    for (int i = 0; i < 100; i++) {
        shared_variable++;
    }
    return NULL;
}

void *decrement_thread(void *arg) {
    for (int i = 0; i < 102; i++) {
        shared_variable--;
    }
    return NULL;
}

int main() {
    pthread_t incrementer, decrementer;

    pthread_create(&incrementer, NULL, increment_thread, NULL);
    pthread_create(&decrementer, NULL, decrement_thread, NULL);

    pthread_join(incrementer, NULL);
    pthread_join(decrementer, NULL);

    printf("Valor final de la variable compartida: %d\n", shared_variable);

    return 0;
}
