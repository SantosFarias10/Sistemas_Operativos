# Virtualización de la CPU

![Ejercicio 1](../VirtualizacionCPU/imagenes/ej1cpu.png)

**Línea 1**: Es un proceso que está corriendo en una sola CPU. Fijate que Walltime > CPUTime porque hay pérdida de tiempo en cambios de contexto y de cómputo dentro del kernel.

**Línea 2**: Son dos procesos corriendo cada uno en su core o CPU y esto muestra que al menos hay dos cores (núcleos). Notar que lanzar 2 procesos tarda lo mismo que uno solo, luego, hay DOS unidades de cómputo.

**Línea 3**: Es un Proceso que está corriendo en una sola CPU. Walltime > CPUTime probablemente por procesos previamente activos, los cuales hacen que el walltime sea mayor que en la línea 1, pero no así el CPUTime.

**Línea 4**: En un mismo CPU se corren 4 procesos (simultáneamente) que tardan lo mismo (2 cores). Esto refuerza la hipótesis de la línea 2, que tenemos dos núcleos.

**Línea 5**: Se corren 3 procesos. Se distribuye la carga de los 3 procesos entre los 2 núcleos para que ningún núcleo quede inactivo (*fairness*). Se busca que todos los procesos terminen a la vez.

**Línea 6**: Se puede asumir que en cada core corre un proceso pi que compite con otros procesos que alargan el walltime.

**Línea 7**: Asumimos que empiezan a trabajar un proceso en cada núcleo, todos ellos compiten con otros procesos y tarda más en terminar lo cual genera que (por ejemplo) el último tarde 7.75. Procesos previos agregan carga de procesado y por lo tanto aumentan el walltime.

Por lo tanto, el sistema tiene 2 núcleos.

---

![Ejercicio 2](../VirtualizacionCPU/imagenes/ej2cpu.png)

El cputime > walltime ya que tenemos muchos hilos dentro del proceso, cada hilo ejecuta en un núcleo distinto. Por lo que el SO acumula todos los tiempos de CPU y los suma.

Recordar que el walltime es el tiempo total transcurrido desde que se inicia hasta que termina el proceso, incluyendo la espera por otros procesos en la CPU, y el cputime es el tiempo total que los núcleos de la CPU han pasado ejecutando instrucciones del programa.

---

![Ejercicio 3](../VirtualizacionCPU/imagenes/ej3cpu.png)

* **user < real**: Por la demora de la trap y del inverso al trap (volver al usuario) añadida al tiempo de usuario porque el SO antes de la ejecución del programa hace de resource manager, ya en las syscalls gastas algo.

* **user = real**: En procesos de un solo hilo sin system calls (con ínfima cantidad de syscalls).

* **real < user**: Por proceso multihilos, cada hilo ejecuta en un core distinto por lo tanto se suman todos para dar con el tiempo usuario.

---

![Ejercicio 4](../VirtualizacionCPU/imagenes/ej4cpu.png)

**(a)** Como en `fork` se hace una copia del programa original, incluyendo el estado, entonces el valor de `x` en el hijo sera 100.

**(b)** Una vez llamado `fork` sus estados se vuelven "independientes" por lo que no depende de si el que la cambia es padre o hijo.

**(c)** Sigue todo igual ya que tanto como el padre como el hijo se respaldan los registros al hacer trap y return from trap.

---

![Ejercicio 5](../VirtualizacionCPU/imagenes/ej5cpu.png)

1. Inicio:
    * Hay un proceso (Digamosle P0).
    * Este proceso ejecuta `printf("a\n")`, lo que imprime un `a`.

2. Despues del primer `fork()`:
    * `fork()` crea un nuevo proceso. Ahora hay 2 procesos en total: P0 y P1.
    * Ambos procesos (P0 y P1) ejecutan `printf("a\n")`, lo que imprime `a` 2 veces (una vez en cada proceso).

3. De momento tenemos:
    * `a` se ha impreso 1 (Inicio) + 2 (Despues del primer `fork()`) = 3 veces.

4. Despues del segundo `fork()`:
    * Cada uno de los dos procesos (P0 y P1) crea un nuevo proceso. Ahora hay cuatro procesos en total: P0, P1, P2 y P3.
    * Cada uno de los cuatro procesos ejecuta `printf("a\n")`, lo que imprime `a` 4 veces (una vez por cada proceso).

5. De momento:
    * `a` se ha impreso 3 (previo) + 4 (despues del segundo `fork()`) = 7 veces.

6. Despues del tercer `fork()`:
    * Cada uno de los cuatro procesos (P0, P1, P2 y P3) crea un nuevo proceso. Ahora hay ocho procesos en total: P0, P1, P2, P3, P4, P5, P6 y P7.
    * Cada uno de los ochos procesos ejecuta `printf("a\n")`, lo que imprime `a` 8 veces (una vez por cada proceso de vuelta xd).

7. De momento:
    * `a` se ha impreso 7 (previo) + 8 (despues del tercer `fork()`) = 15 veces.

---

![Ejercicio 6](../VirtualizacionCPU/imagenes/ej6cpu.png)

Ninguna, ya que el `execv()` de funcionar correctamente, (toda syscall tiene posibilidad de fallar) no returnea a esta función, evitando que la linea del `printf` se ejecute.

No debería imprimir ninguna `a` (si es que no falla el comando), ya que el comando `execv(args[0], args)` reemplaza el programa del proceso actual por el del programa que ejecuta como argumento en `args[0]`. Si falla, imprime 1 `a`.

---

![Ejercicio 7](../VirtualizacionCPU/imagenes/ej7cpu.png)

