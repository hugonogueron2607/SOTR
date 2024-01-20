#include <stdio.h>
#include <pthread.h>


int var_com = 0;

void *thread1_fn(void *arg) {

    var_com = 42;

    pthread_exit(NULL);
}

void *thread2_fn(void *arg) {

    var_com = 17;

    pthread_exit(NULL);
}

int main() {
    pthread_t thread1, thread2;


    pthread_create(&thread1, NULL, thread1_fn, NULL);
    pthread_join(thread1, NULL);
    printf("Hilo 1 envio el valor de: %d\n",var_com);


    pthread_create(&thread2, NULL, thread2_fn, NULL);
    pthread_join(thread2, NULL);
    printf("Hilo 2 envio el valor de: %d\n", var_com);

    return 0;
}
