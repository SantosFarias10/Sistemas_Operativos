#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv) {

    if(argc != 2) {
        printf("ERROR: el programa debe recibir un argumento\n");
        return 0;
    }

    int length = atoi(argv[1]);  // atoi transforma un string en un int
    if(length < 1) {
        printf("ERROR: el argumento debe ser un numero entero mayor a cero\n");
        return 0;
    }    

    int sem_hijo = sem_search(0);
    int sem_padre = sem_search(1); 

    int f = fork();

    if(f == -1) {
        printf("ERROR: fallo el fork\n");
        return 0;
    } else if(f == 0) { //caso del hijo
        for (unsigned int i = 0; i < length; i++) {
          sem_down(sem_hijo);
          printf("\tpong\n");
          sem_up(sem_padre);
        }
        return 1;
    } else {            //caso del padre
        for (unsigned int i = 0; i < length; i++) {
          sem_down(sem_padre);
          printf("ping\n");
          sem_up(sem_hijo);
        }
    }
    
    wait(0); //esperamos a que el hijo termine
    sem_close(sem_hijo);
    sem_close(sem_padre); 

    return 1;
}

