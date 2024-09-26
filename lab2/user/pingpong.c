#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void ping(){

    sem_down(1);
    printf("ping\n"); // \r para que haga un salto de linea con sangria
    sem_up(1);
}

void pong(){
    sem_down(1);
    printf("    pong\n");
    sem_up(1);
}

int main(int argc, char **argv) {

    if(argc != 2){
        printf("ERROR: el programa debe recibir un argumento\n");
        return 0;
    }

    int length = atoi(argv[1]);  // atoi
    if (length < 1){
        printf("ERROR: el argumento debe ser un numero entero mayor a 0\n");
        return 0;
    }    
    
    sem_open(1,1); // abrimos semaforo de nombre 1 con un solo recurso disponible

    int i = 0;
    

    int f = fork();

    if(f == -1){
        exit(0);
    }else if(f == 0){ //caso del hijo
        while(i < length){
            pong();
            i++;
        }

    }else{            //caso del padre
        while(i < length){
            ping();
            i++;
        }
    }

  

    sem_down(1);
    sem_up(1);
    exit(0);
}