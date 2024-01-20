#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_HIJO 0
#define SEM_PADRE 1

int main(int argc, char *argv[]){
    int i=10, semid, pid;
    struct sembuf operacion;
    key_t llave; //llave de sistema
    //peticion de id con dos semaforos
    llave=ftok(argv[0],'K');//crea token de llave
    if ((semid=semget(llave,2,IPC_CREAT | 0600))==-1){
        printf("error al crear semaforo\n");
        exit(-1);
    }
    //se inicializan los semaforos, cerramos semaforo del hijo
    semctl(semid,SEM_HIJO,SETVAL,0);
    //se abre el semaforo del proceso padre
    semctl(semid,SEM_PADRE,SETVAL,1);
    //se crea el proceso hijo
    if((pid=fork())==-1){
        printf("error al crear hijo\n");
        exit(-1);
    }
    else if (pid==0){//proceso hijo
        while(i){
            //cerramos el semaforo del proceso hijo
            operacion.sem_num=SEM_HIJO;
            operacion.sem_op=-1;
            operacion.sem_flg=0;
            semop(semid,&operacion,1);
            printf("proceso hijo: %d\n",i--);
            //se abre al semaforo padre
            operacion.sem_num=SEM_PADRE;
            operacion.sem_op=1;
            semop(semid,&operacion,1);
        }
        //borrado de semaforos
        semctl(semid,0,IPC_RMID,0);
    }else{
        //codigo padre
        operacion.sem_flg=0;
        while(i){
            //se cierra el semaforod el padre
            operacion.sem_num=SEM_PADRE;
            operacion.sem_op=-1;
            semop(semid,&operacion,1);
            printf("Proceso padre: %d\n",i--);

            //abrimos el semaforo del hijo
            operacion.sem_num=SEM_HIJO;
            operacion.sem_op=1;
            semop(semid,&operacion,1);
        }
        //borrado del semaforo
        semctl(semid,0,IPC_RMID,0);
    }
}
