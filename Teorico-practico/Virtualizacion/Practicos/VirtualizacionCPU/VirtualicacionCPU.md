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

1. En el primer programa vemos que toma dos argumentos: Un `int argc` y un arreglo de punteros que apuntan a cadenas `argv`. Primero aparece una guarda que verifica si el entero `argc` es mayor que cero luego de ser decrementado en 1 (osea, verifica que haya argumentos xd). En caso de ser negativo el programa no hace nada. En caso de ser positivo, la posición indicada por `argc` se establece en NULL (Para asegurar que la lista de argumenos termine correctamente) y se llama a `execvp()` con `argv[0]` como primer parametro y `argv` como segundo parametro lo que reemplaza el proceso actual con el comando especificado. Si `execvp` tiene exito, el programa original nunca sigue ejecutandose, si falla el programa termina. Basicamente el programa toma el tamaño de un arreglo y el arreglo en si, el cual contiene punteros a cadenas que indican el nombre del programa y luego sus argumentos, esto se deduce al ver la llamada `execvp()` quien como argumentos pide el nombre del programa para buscarlo en los directorios y ademas un arreglo que contenga nombre, sus argumentos y un puntero a NULL en su última posición.

2. El segundo programa vemos una guarda que verifica que `argc <= 1` (si no ha suficientes argumentos termina el programa). Llama a `fork()` para crear un proceso hijo, si el `fork()` falla (`rc < 0`) retorna `-1`. Si `fork()` tiene exito entonces en el proceso hijo (cuando `rc == 0`) simplemente retorna 0 y termina. En el proceso padre (`rc > 0`) ejecuta `execvp()` con el camando y sus argumentos. Hace exatamente lo mismo que el primer programa, pero este crea un proceso hijo que no hace nada y un proceso padre que ejecuta el comando xd.

---

![Ejercicio 8](../VirtualizacionCPU/imagenes/ej8cpu.png)

1. Primero cierra la salida estandar (`SRDOUT_FILENO`) con `close()`, luego abre el archivo `salida.txt` (con `open()`) en el filedescriptor 1, porque es el que más arriba esá libre. Por ultimo `printf()` imprime "¡Mirá mamá salgo por un archivo!" en el file descriptor 1 (siempre segun la man page).

2. Primero abre el archivo `salida.txt` con `open()`, luego cierra la salida estandar (`STDOUT_FILENO`) con `close()`, duplica el file descriptor fd (el que abrio el archivo `salida.txt`) en el primer file descriptor libre con `dup()` y por ultimo `printf()` imprime en `salida.txt` "¡Mirá mamá salgo por un archivo!".

---

![Ejercicio 9](../VirtualizacionCPU/imagenes/ej9cpu.png)

Se ejecuta un bucle infinito y en cada iteración llama a `fork()`, `fork()` crea un proceso idéntico al proceso padre. Tanto el proceso padre como el nuevo proceso continúan ejecutando el bucle, creando aún más procesos en cada iteración, lo que provoca un cremiento exponencial del número de procesos hasta que el sistema agota su capacidad de crear nuevos procesos, lo que puede llevar a la congelación o intestabilidad del sistema.

---

![Ejercicio 10](../VirtualizacionCPU/imagenes/ej10cpu.png)

* **Running a Blocked**: Sin bloqueo por I/O, los procesos podrían esperar eventos I/O, lo que causaría ciclos de CPU desperdiciados.

* **Blocked a Ready**: Sin retorno a Ready despues de I/O, los procesos bloqueados nunca podrían reanudar su ejecución, quedando atascados.

* **Running a Ready**: Sin capacidad de interrumpir o pausar temporalmente un proceso o hilo, los procesos nunca serían interrumpidos, monopolizando la CPU.

* **Ready a Running**: Sin planificación de procesos, los procesos nunca ejecutarían, y el sistema se paralizaría.

---

![Ejercicio 12](../VirtualizacionCPU/imagenes/ej12cpu.png)

* **(a)** Es posible que $user + sys = real$.

**Verdadero**, ya que si, por ejemplo, tenes 10k de procesos con 1 nucleo, entre que se reparten la CPU y avanza el proceso; el tiempo real sigue avanzando mientras que $user + sys$ avanza muy poco.

* **(b)** dos procesos no pueden usar la misma dirección de memoria virtual.

**Falso**. Ambos pueden usar la misma dirección de memoria virtual, pero que es mapeada a distintas ubicaciones en la memoria física (los proceso no se dan cuenta).

* **(c)** Para guardar el estado del proceso es necesario salvar el valor de todos los registros del microprocesador.

**Verdadero**, (y mas) también multiplexo en espacio la ram, stack/heap.

* **(d)** Un proceso puede ejecutar cualquier instrucción de la ISA.

**Falso**. Instrucciones privilegiadas (del kernel) no puede ser ejecutadas desde el user spaces (modo usuario).

* **(e)** Puede haber traps por timer sin que esto implique cambiar de contexto.

**Verdadero**. En general hay millones de interrupts por timer pero se puede mantener el proceso (cuando hay uno por ejemplo).

* **(f)** `fork()` devuelve 0 para el hijo, porque ningun proceso tiene PID 0.

**Verdadero**. Que devuelva 0 no implica que el proceso hijo tenga PID 0 xd.

* **(g)** Las syscall `fork()` y `execv()` están separadas para poder redireccionar los descriptores de archivo.

**Verdadero**. Para eso se separaron xd.

* **(h)** Si un proceso padre llama a `exit()` el proceso hijo termina su ejecución de manera inmediata.

**Falso**. El proceso hijo queda colgando de init. Aparte una vez que hiciste `fork()` tanto el proceso padre como hijo son dos procesos diferentes.

* **(i)** Es posible pasar información de padre a hijo a traves de `argv`, pero el hijo no puede comunicar información al padre ya que son espacios de memoria independientes.

**Falso** Hay comunicación padre -> hijo -> padre, como: padre -> hijo se hace con `argv` y `argc`, y hijo -> padre se hace con el `return` y `waitpid()`.

* **(j)** Nunca se ejecuta el código que está después de `execv()`

**Falso**. Contraejemplo: Cuando hay un error en el `execv()`, si se ejecuta el código que está después.

* **(k)** Un proceso hijo que termina, no se puede liberar de la tabla de procesos hasta que el padre no haya leido el exit status via `wait()`.

**Verdadero**, estos son los procesos Zombie, donde el padre se queda esperando el estado de salida de su hijo.

---

## Políticas

![Ejercicio 13](../VirtualizacionCPU/imagenes/ej13cpu.png)

FIFO = First in First Out

|  0  |  5  | 10  | 15  | 20  | 25  | 30  | 35  | 40  | 45  | 50  | 55  | 60  | 65  | 70  | 75  | 80  | 85  | 90  | 95  |
|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| C  | C  | C  | B  | B  | B  | A  | A  | A  | A  | A  | A  | A  | A  | —  | —  | —  | —  | —  | —  |

SJF (Shortest Job First)

|  0  |  5  | 10  | 15  | 20  | 25  | 30  | 35  | 40  | 45  | 50  | 55  | 60  | 65  | 70  | 75  | 80  | 85  | 90  | 95  |
|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| C  | C  | C  | B  | B  | B  | A  | A  | A  | A  | A  | A  | A  | A  | —  | —  | —  | —  | —  | —  |

---

![Ejercicio 14](../VirtualizacionCPU/imagenes/ej14cpu.png)

* **STCF**

| **Tiempo**  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  |
|-------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| **Running** | $B^{3}$ | $B^{2}$ | $B^{1*}$ | $A^{4}$ | $C^{1*}$ | $A^{3}$ | $A^{2}$ | $A^{1}$ |    |    |    |    |    |    |    |    |    |    |    |    |
| **Arribos** | B  |    | A   |    | C   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
| **Ready**   |    | A  |     |    |     | A  | A  |    |    |    |    |    |    |    |    |    |    |    |    |    |

* Se compara con A y gana B por tener menos $T_{CPU}$, entonces sigue ejecutando B.

* Igual que antes, C tiene menos $T_{CPU}$ que A, C gana por política STCF. A queda en Ready esperando a que termine C.

**Desempate**: El que estaba corriendo que siga corriendo. Es caro de cambiar entre procesos pa.

| **Proceso** | $T_{arrival}$ | $T_{CPU}$ | $T_{firstrun}$ | $T_{completion}$ | $T_{turnaround}$ | $T_{response}$ |
|------------|----------------|------------|----------------|------------------|----------------|--------------|
| **A**      | 2              | 4          | 3              | 7                | 5              | 0            |
| **B**      | 0              | 3          | 0?             | 2?               |                |              |
| **C**      | 4              | 1          | 4?             | 4?               |                |              |

* **RR** (Los procesos solo se ejecutan durante 2 quantos, luego van al final de la cola).

| **Tiempo**    | 0   | 1   | 2   | 3   | 4    | 5   | 6   | 7   | 8   | 9   | 10  | 11  | 12  | 13  | 14  | 15  |
|--------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| **Running**  | $B^{3}$ | $B^{2}$ | $A^{4}$ | $A^{3}$ | $C^{1*}$ | $B$ | $A^{2}$ | $A^{1}$ |    |    |    |    |    |    |    |    |
| **Arribos**  | B  |    | A  |    | C  |    |    |    |    |    |    |    |    |    |    |    |
| **Ready** ¿Cola? |    | B  | B  | B  | **B** <br> **A** | A  |    |    |    |    |    |    |    |    |    |    |
| **Qs Restantes** | $B:3$ | $B:2$ | $A:4$ | $A:3$ | $C:1$ | $B:1$ | $A:2$ | $A:1$ |    |    |    |    |    |    |    |    |

* C tiene $T_{CPU} = 1$, sólo dura 1 quanto. Había simultaneidad entre B y C, nos decidimos por el C por política de prioridad: el proceso que entra se ejecuta. Esta política debe ser consistente para cada paso recurrente.

Posibles plíticas para tratar el desempate:

* Podrías darle prioridad a procesos que recién aparecen o arriban.

* Decido ejecutar el que ejecute antes. Rezando que está cacheado o algo asi.

* Decido ejecutar el C porque no se ejecutó ninguna vez, para ser justo.

---

![Ejercicio 15](../VirtualizacionCPU/imagenes/ej15cpu.png)

| **Algoritmo** | **Batch/Interactivo** | ¿Necesita saber ($T_{cpu}$)? |
|--------------|------------------------|----------------------------------|
| **FIFO**     | Batch                  | No                               |
| **SJF**      | Batch                  | Sí                               |
| **RR**       | Interactivo            | No                               |
| **STCF**     | 50/50                  | Sí                               |
| **MLFQ**     | Interactivo            | No                               |

Interactivo = Tiempo de respuesta corto.

¿Porque 50/50 el STCF? No corta por quanto (no es interactivo en ese sentido), pero si es interactivo si llegan procesos cortos.

---

![Ejercicio 16](../VirtualizacionCPU/imagenes/ej16cpu.png)

$T_{CPU}$ total de cada proceso es: A = 6, B = 14, C = 6.

**RR** (Q = 2) los procesos solo se ejecutan durante 2 quantos, luego van al final de la cola.

| **Tiempo**    | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 |
|---------------|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| **Running (CPU)**  | $A^{3}$ | $A^{2}$ | $C^{1}$ | $B^{8}$ | $B^{7}$ | $A^{1}$ | $B^{6}$ | $B^{5}$ | $C^{2}$ | $C^{1}$ | $B^{4}$ | $B^{3}$ | $A^{2}$ | $A^{1}$ | $B^{2}$ | $B^{1}$ | $C^{1}$ | $B^{6}$ |$B^{5}$ | $A^{1}$ | $B^{4}$ | $B^{3}$ | $C^{2}$ | $C^{1}$ | $B^{2}$ | $B^{1}$ |
| **Blocked (I/O)**  |    |    | C  | C  | A  | A  | A  | A  | C  |    | C  |    | C  | C  |    |    | B, A | A, C |    | C  | C  | C  | C  |    |    |
| **Arribos**   | A  |    | C  | B  |    |    | C  |    |    |    |    | A  |    |    |    |    |    |    |    |    |    |    | C  |    |    |    |
| **Ready Cola**| C  | B  | A  | A  | A  | A  | B  | C  | B  | B  | A  | B  | B  |    | C  |    |    | C   | B   | B  |    |    | B  | B  | B  |    |
| **Término**   |    |    |    |    |    |    |    |    |    |    |    |    |    | A  |    |    |    |    |    | A  |    |    |    |    | A  | C  |

Política: Si un proceso A termina su quanto al MISMO tiempo que arriba B, el proceso B va primero en la cola Ready.

---

![Ejercicio 17](../VirtualizacionCPU/imagenes/ej17cpu.png)

| **T** | 0  | 1  | 2   | 3   | 4   | 5   | 6   | 7   | 8    | 9    | 10   | 11   | 12  | 13  | 14  | 15  | 16  | 17  | 18  | 19  | 20  |
|-------|----|----|-----|-----|-----|-----|-----|-----|------|------|------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| **Q₁**| **A**  | **B**  | **C**  |    | **D**    |     |     |     | **E**    |      |      |      |     |     |     |     |     |     |     |     |     |
| **Q₂**|    | A  | BA  | CB**A** | CBA | DCB**A**| DC**B** | DCB | EDCB* | ED**C**  | ED**C**  | E**D**   | ED*  | **E**   | **E**   |     |     |     |     |     |     |
| **Q₄**|    |    |     |     |     |     | A    | A   | A    | A    | A    | A    | CA  | CA  | CA  | EC**A** | EC**A** | EC**A** | EC*  | EC*  | E*   |
| **Q₈**|    |    |     |     |     |     |     |     |      |      |      |      |     |     |     |     |     |     |     |     |     |

En **negrita** estan marcados los procesos que se ejecutan en el tiempo.

Los que tengan un * a la derecha cada proceso en en el tiempo que termina.

Si un proceso ejecuta la cantidad (n) de quantos correspondiente a cada cola de prioridad, baja a la siguiente cola de prioridad. Esta ejecución puede ser incontinua. ->politica MLFQ, acumula las ejecuciones del proceso en la cola para mandarlo a una cola de menor prioridad.

---

![Ejercicio 18](../VirtualizacionCPU/imagenes/ej18cpu.png)

* **(a)**

**Falso**. El STCF compara los procesos entrantes con los actuales y elige cual continuar, independientemente si el proceso actual "gasto" su quantum o no.

* **(b)**

**Falso**. En el caso de que al SJF le llegue un proceso largo antes de los cortos, tiene el mismo tarnaround time que el FCFS.

* **(c)**

**Verdadero**. Significa que cada proceso puede ejecutarse sin interrupciones hasta que termine. En este caso, el comportamiento se asemeja al de FCFS, donde los procesos se ejecutan en el orden en que llegan sin interrupciones.

* **(d)**

**Verdadero**. Si hay constantemente procesos de alta prioridad en la cola, los procesos de baja prioridad pueden no recibir suficiente tiempo de CPU para ejecutarse, lo que puede llevar a que nunca terminen.

* **(e)**

**Verdadero**. El "Allot" se gasta igual independientemente si el proceso hace I/O o no.