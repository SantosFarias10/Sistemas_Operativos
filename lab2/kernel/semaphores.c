#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

#define MAX_SEMAPHORES 10

struct semaphore {
  int value;
  struct spinlock lock;
};

struct semaphore semaforo[MAX_SEMAPHORES];          //Semaforo General

//Busca un semaforo disponible (semaforo que no este cerrado)
int sem_search(int value) {
  int sem = 0;
  while(sem < MAX_SEMAPHORES && sem_open(sem, value) == 0) { //Va a salir si sem es menos al max de semaforos y si lorga abrirlo
    sem++;
  }
  
  if(sem == MAX_SEMAPHORES) {
    return -1;
  }

  return sem;
}

//funcion para inicializar los semaforos en -1  ¡¡¡¡llamar al princio del main!!!!
void sem_init() {
  for (int i = 0; i < MAX_SEMAPHORES; i++) {
    semaforo[i].value = -1;                         //Inicializamos todos los sem en -1 ("no existen")
    initlock(&(semaforo[i].lock), "Semaphore");                //Inicializamos el spinlock y le damos nombre de Semaphore (hay que ponerlo en sem_init junto con la asignacion del -1 porque ahi se inicializa el semaforo)
  }
}

int sem_open(int sem, int value) {
  if (sem < 0 || sem >= MAX_SEMAPHORES || semaforo[sem].value != -1 || value < 0) {
    return 0;                                   //El semaforo es invalido, retornamos cero que es error
  }

  acquire(&(semaforo[sem].lock));                              //Bloqueamos 
  semaforo[sem].value = value;                               //Le asigamos el valor 
  release(&(semaforo[sem].lock));                              //Desbloqueamos

  return 1;                                       //Retornamos uno que es exito
}

int sem_close(int sem) {
  if (sem < 0 || sem >= MAX_SEMAPHORES) {
    return 0;
  }
  
  acquire(&(semaforo[sem].lock));
  semaforo[sem].value = -1;                                     //Le asignamos el valor -1
  release(&(semaforo[sem].lock));

  return 1;
}

int sem_up(int sem) {
  if (sem < 0 || sem >= MAX_SEMAPHORES || semaforo[sem].value == -1) {
    return 0;
  }

  acquire(&(semaforo[sem].lock)); 
  if(semaforo[sem].value == 0) {
    wakeup(&(semaforo[sem]));                                     //Despertamos el proceso
  }
  (semaforo[sem].value)++;                                       //Incrementamos el valor
  release(&(semaforo[sem].lock));
 
  return 1;
}

int sem_down(int sem) {
  if (sem < 0 || sem >= MAX_SEMAPHORES || semaforo[sem].value == -1) {
    return 0;
  }

  acquire(&(semaforo[sem].lock));
  if(semaforo[sem].value == 0){
    sleep(&(semaforo[sem]), &(semaforo[sem].lock));
  }
  semaforo[sem].value--;
  release(&(semaforo[sem].lock));

  return 1;
}

