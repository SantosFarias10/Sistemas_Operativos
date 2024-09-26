#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"  // Include the header file where struct proc is defined
#include <stdbool.h>

#define MAX_SEMAPHORES 256

struct semaphore {
  int value;
  struct spinlock lock;
  int maxValue;
};

struct semaphore semaforo[MAX_SEMAPHORES];          //Semaforo General

//funcion para inicializar los semaforos en -1  ¡¡¡¡llamar al princio del main!!!!
void sem_init() {
  for (int i = 0; i < MAX_SEMAPHORES; i++) {
    semaforo[i].value = -1;                         //Inicializamos todos los sem en -1 ("no existen")
  }
}

int sem_open(int sem, int value) {
  if (sem < 0 || sem >= MAX_SEMAPHORES || semaforo[sem].value >= 0 || value <= 0) {
      return 0;                                   //El semaforo es invalido, retornamos cero que es error
  }
  
  acquire(&semaforo[sem].lock);                              //Bloqueamos
  initlock(&semaforo[sem].lock, "Semaphore");                //Inicializamos el spinlock y le damos nombre de Semaphore
  semaforo[sem].value = value;                               //Le asigamos el valor 
  semaforo[sem].maxValue = value;                            //Definimos el valor maximo
  release(&semaforo[sem].lock);                              //Desbloqueamos
  return 1;                                       //Retornamos uno que es exito
}


int sem_close(int sem) {
  if (sem < 0 || sem >= MAX_SEMAPHORES) {
    return 0;
  }
  
  acquire(&semaforo[sem].lock);
  semaforo[sem].value = -1;                                     //Le asignamos el valor -1 (no se si se podria hacer free?)
  release(&semaforo[sem].lock);

  return 1;
}

int sem_up(int sem) {
  if (sem < 0 || sem >= MAX_SEMAPHORES || semaforo[sem].value >= semaforo[sem].maxValue) {
    return 0;
  }
  acquire(&semaforo[sem].lock);

  if(semaforo[sem].value == 0){
    wakeup(&semaforo[sem]);                                     //Despertamos el proceso
  }
  semaforo[sem].value++;                                       //Incrementamos el valor
  release(&semaforo[sem].lock);

  
  return 1;
}

int sem_down(int sem) {
  if (sem < 0 || sem >= MAX_SEMAPHORES) {
        return 0;
  }

  acquire(&semaforo[sem].lock);

  if(semaforo[sem].value == 0){
    sleep(&semaforo[sem], &semaforo[sem].lock);
  }
  if(semaforo[sem].value > 0){
    semaforo[sem].value--;
  }

  release(&semaforo[sem].lock);

  return 1;
}