#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t abuelo_pid, padre_pid, hijo_pid;

    printf("Soy el abuelo (%d).\n", getpid());

    padre_pid = fork();

    if (padre_pid < 0) {
        perror("Error al crear el proceso padre");
        return 1;
    }

    if (padre_pid == 0) {
        printf("Soy el padre (%d).\n", getpid(), getppid());
        hijo_pid = fork();

        if (hijo_pid < 0) {
            perror("Error al crear el proceso hijo");
            return 1;
        }

        if (hijo_pid == 0) {
            printf("Soy el hijo (%d).\n", getpid(), getppid());
        }
    } else {
        wait(NULL); 
    }

    return 0;
}
