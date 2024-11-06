# Lab 3 🚬

---
## Primera Parte: Estudiando el Planificador de xv6-riscv

### 1.  ¿Qué política de planificación utiliza `xv6-riscv` para elegir el próximo proceso a ejecutarse?
En xv6 se usa la técnica en la que interrupciones de tiempo en hardware "Hardware Timer's Interrupts" que manejan el cambio de contexto "Context Switches". Esta política de planificación se llama RoundRobin.

### 2.  ¿Cuáles son los estados en los que un proceso puede permanecer en xv6-riscv y qué los hace cambiar de estado?
Los estados de los procesos son runnable, running, sleeping, zombie, used y unused.
Y están definidos en la estructura de procesos como, p->state == RUNNABLE, RUNNING, SLEEPING, ZOMBIE, UNUSED, USED.

Los cambios de estado ocurren por las siguientes razones:

- Al **crearse un proceso** se crea con el estado _runnable_.

- Al **planificarse**, los procesos pasan de _runnable_ a _running_.

- Al **esperar una operación** de IO, pasa de _running_ a _sleeping_. Y al finalizar la operación de IO, pasa a _runnable_.

- Al suceder una **interrupción de tiempo** o lo equivalente a que pase el quantum del planificador, el proceso pasa de _running_ a _runnable_.

- Al **finalizar un proceso**, este pasa de _running_ a _zombie_ hasta que el proceso padre libera su memoria para pasar al estado _unused_.

### 3.  ¿Qué es un *quantum*? ¿Dónde se define en el código? ¿Cuánto dura un *quantum* en `xv6-riscv`?
Un **Quantum** es el tiempo transcurrido entre los Hardware Timer's Interrupts. 
En xv6 el código se define en `kernel/start.c` específicamente en la función `timerinit()`, acá preguntándole a **CLINT** (core-local interruptor) se obtiene en conjunto con el quantum, donde se generara la interrupción.
Este quantum dura 1/10 de un segundo, o aproximadamente 10ms, según está definido en la variable `int interval = 1000000`.

### 4. ¿En qué parte del código ocurre el cambio de contexto en `xv6-riscv`? ¿En qué funciones un proceso deja de ser ejecutado? ¿En qué funciones se elige el nuevo proceso a ejecutar?
El cambio de contexto ocurre en `kernel/proc.c`, en la función `sched()` y `scheduler()`, al llamarse a `swtch()`.

Las funciones en las que un proceso deja de ser ejecutado son:

- **exit()**, porque sale de un proceso dejándolo en modo `zombie`, esto puede suceder mientras el proceso estaba en `running`.

- **yield()**, porque "deschedula" un proceso dejándolo en `runnable`.

- **sleep()**, porque pone en estado `sleep` a un proceso, por ejemplo al esperar un IO.

- **kill()**, porque mata al proceso, que puede estar en `running`.

- **wait()**, porque pone a dormir al proceso padre, hasta que el proceso hijo "exits".

Las función donde se elige el nuevo proceso a ejecutar es:

- **scheduler()**, porque este elige qué proceso se ejecuta.

### 5. ¿El cambio de contexto consume tiempo de un *quantum*?
El cambio de contexto ocurre al principio del quantum. Esto afecta muy levemente al tiempo real que usa un proceso, ya que el cambio de contexto consume muy poco tiempo, y por ende muy poco tiempo del quantum es dedicado al cambio de contexto; el tiempo real que se usa para el proceso en cada quantum sería de (quantum_time - cambioDeContexto_time).
Esto significa que puede haber casos donde, si el quantum es menor al tiempo que necesita el cambio de contexto, se gastaría todo en el cambio de contexto y no podría ejecutar el proceso.

---
## Segunda Parte: Medir operaciones de cómputo y de entrada/salida
[Excel con las mediciones](https://docs.google.com/spreadsheets/d/1TeCbiHJukHqmnZAdiCtsvBO3jW3mBObXab9GsnH8lzc/edit?gid=0#gid=0)

---

## Experimento 1: ¿Cómo son planificados los programas iobound y cpubound?

### 1. Describa los parámetros de los programas cpubench e iobench para este experimento (o sea, los define al principio y el valor de N. Tener en cuenta que podrían cambiar en experimentos futuros, pero que si lo hacen los resultados ya no serán comparables).

#### Los parámetros de **Cpubench**: 

- CPU_MATRIX_SIZE = "Es el tamaño de la matriz a multiplicar" 

- CPU_EXPERIMENT_LEN = "Es la cantidad de veces que se repite la multiplicación de la matriz"

- MESURE_PERIOD = "Es la variable por la que se multiplica la métrica, para que el resultado quede cada 10ms"

- kops_matmul = "Es la cantidad de operaciones que se hacen en la multiplicación de las matrices, dividido por mil ya que puede ser un número más grande que el número máximo de un int ($(2^{31}) - 1 = 2,147,483,647$)"

- start_tick = "Es la variable que almacena el tick en el que comienza el proceso"

- end_tick = "Es la variable que almacena el tick en el que finaliza el proceso"

- elapsed_ticks = "Es la variable que almacena la cantidad de ticks que tarda un proceso en terminar"

- total_cpu_kops = "Es la variable que almacena la cantidad de operaciones que se realizan en la multiplicación de la matriz, dividido por mil"

- metric = "Es la variable que definimos para ver cómo vamos a comparar la cantidad de operaciones"

- N = "Es la cantidad de veces que se repite el proceso de cpubench"

- pid = "Es el número de identificación de un proceso"

#### Los parámetros de **Iobench**:

- IO_OPSIZE = "Es la cantidad de bytes que se leen/escriben del buffer data" 

- IO_EXPERIMENT_LEN = "Es la cantidad veces que se llama a las funciones write y read"

- rfd = "Es el file descriptor del archivo de lectura"

- wfd = "Es el file descriptor del archivo de escritura"

- MEASURE_PERIOD = "Es la variable por la que se multiplica la métrica, para que el resultado quede cada 10ms"

- data = "Es el buffer que contiene la información que se desea leer o escribir"

- start_tick = "Es la variable que almacena el tick en el que comienza el proceso"

- end_tick = "Es la variable que almacena el tick en el que finaliza el proceso"

- elapsed_ticks = "Es la variable que almacena la cantidad de ticks que tarda un proceso en terminar"

- metric = "Es la variable que definimos para poder comparar la cantidad de operaciones"

- N = "Es la cantidad de veces que se repite el proceso de iobench"

- pid = "Es el número de identificación de un proceso"

### 2. ¿Los procesos se ejecutan en paralelo? ¿En promedio, qué proceso o procesos se ejecutan primero? Hacer una observación cualitativa.
**Sí**, se puede decir que se ejecutan en paralelo, si consideramos que ejecutar en paralelo es que se ejecuten intercaladamente los procesos. Como estamos emulando QEMU con un solo núcleo, el planificador ejecuta un proceso hasta que se le acaba el quantum, para recién ahí ejecutar el siguiente proceso. Es decir, el planificador no espera a que finalice un proceso para ejecutar el siguiente.

El planificador sigue las reglas de RoundRobin, por lo que ejecuta el primero en llegar o más específicamente, el primero que se llamó, ej: `iobench 10 &; cpubench 10 &; cpubench 10 &; cpubench 10 &`; En este caso se planifica primero el iobench, seguido de los otros tres cpubench, pero se terminan de ejecutar los cpubench's antes. 

Entonces, en promedio (_casi siempre en nuestro caso_), se terminan de ejecutar primero los cpubench, ejecutando los iobench recién cuando se completan los cpubench. Creemos que esto se da porque los iobench al realizar operaciones de I/O, el planificador no los tiene en cuenta si están bloqueados, de esa forma siempre planifica a los de cpubench, además al realizar una cantidad mucho mayor de operaciones, solicitan en mucha mayor cantidad la cpu por lo que es más probable que se planifiquen.

### 3. ¿Cambia el rendimiento de los procesos iobound con respecto a la cantidad y tipo de procesos que se estén ejecutando en paralelo? ¿Por qué?
Si se ejecuta **un iobench solo**, desperdicia ticks durante su ejecución usando más de los que necesita, esto se da porque está bloqueado, esperando la operación I/O, alguna de las veces que el planificador busca procesos para ejecutar.

Si se ejecutan **dos iobench** cada uno tarda una menor cantidad de ticks en finalizar, por lo que es más óptimo en cuestión de tiempo ya que el planificador va cambiando entre ambos procesos a la hora de elegir cuál planificar.

Si se va aumentando la cantidad de iobenchs la cantidad de ticks empieza a aumentar dependiendo de la cantidad de procesos ejecutándose en paralelo.

Si se mezclan iobench y cpubench, la primera vez que se ejecuta un proceso de iobench le lleva una gran cantidad de ticks terminar de ejecutarse, ya que se empieza a ejecutar cuando es llamado, pero termina luego de que finalice la ejecución de todos los procesos cpubench, por lo que afecta mucho el rendimiento de cada primer llamada de iobench, pero luego, como para ejecutar los iobench se espera a que terminen todos los cpubench, se comportan como si solo hubiera iobenchs.

### 4. ¿Cambia el rendimiento de los procesos cpubound con respecto a la cantidad y tipo de procesos que se estén ejecutando en paralelo? ¿Por qué?
Sí, cambia el rendimiento de los procesos cpubound. 
Con respecto a la cantidad, mientras más cpubounds haya, más tarda en finalizar los procesos, debido a que se planifican los otros procesos cpubound.
Con respecto al tipo de procesos que se ejecutan en paralelo, de ser los iobench, estos ocupan ticks por lo que se extiende el tiempo de ejecución de los cpubound, bajando su rendimiento, pero no tanto, ya que al estar en el estado de bloqueado, el planificador no los tiene en cuenta.

### 5. ¿Es adecuado comparar la cantidad de operaciones de cpubound con la cantidad de operaciones iobound?
Con nuestra métrica, sí es posible, ya que nosotros no vemos el tipo de las operaciones, si no la cantidad de las mismas. Al principio pensamos en poner el promedio de operaciones que hace el proceso cada 10ms, y si el proceso tiene un tiempo de ejecución más grande, su promedio bajaría. Luego, decidimos simplemente hacer que las métricas de Iobench y Cpubench tengan la misma cantidad de dígitos para simplificar a la vista y por ende la comparación. De todas formas, comparar en sí el resultado que nos da la métrica, no tiene mucho sentido ya que no nos dice nada.

---
## Experimento 2: ¿Qué sucede cuando cambiamos el largo del quantum?

### 1. ¿Fue necesario modificar las métricas para que los resultados fueran comparables? ¿Por qué?

Sí, fue necesario modificar las métricas para que los resultados vuelvan a ser comparables. Ya que al ser más chico el quantum, los procesos realizaban más operaciones por quantum y por ende la métrica anterior quedaba muy chica.

### 2. ¿Qué cambios se observan con respecto al experimento anterior? ¿Qué comportamientos se mantienen iguales?

#### Quantum 10mil
Con respecto al experimento anterior, el quantum, al ser más chico, el planificador tiene más probabilidad de ejecutar un proceso iobound y por lo tanto hay más probabilidades de que termine un proceso iobound antes de que terminen todos los procesos cpubound, cosa que casi nunca pasaba con el quantum de 100mil.

#### Quantum mil
Con este quantum, lo dicho anteriormente se amplifica más, ya que ahora la probabilidad de que el planificador ejecute un proceo iobound o uno cpubound, son casi iguales, por lo que finalizan los procesos casi que por orden de llegada. Específicamente lo que pasa es que en los procesos iobound, al ponerse en bloqueado esperando a las acciones I/O, hay más probabilidad de que el planificador los agarre cuando dejaron de estar bloqueados. Lo mismo pasa con el quantum de 10mil, pero con menor frecuencia. 

### 3. ¿Con un quantum más pequeño, se ven beneficiados los procesos iobound o los procesos cpubound?

Si el quantum es más chico, los procesos que se benefician más son los iobound debido a que ya no tienen a esperar a que los procesos cpubound finalicen porque tienen mayor probabilidad de ser elegidos por el planificador al haber mayor cantidad de interrupciones, esto hace que los procesos iobound tengan un tiempo de respuesta más chico. En consecuencia, como ahora todos los procesos usan la cpu casi igualitariamente, la cantidad de ticks que les lleva terminar de ejecutarse aumenta en promedio para todos los procesos.🚬

---
**[Repositorio](https://github.com/EmpanadasNqn/Obsidian/tree/main/FaMAF/SistemasOperativos) con resumen hecho en obsidian donde fuimos anotando algunas cosas extra sobre el laboratorio.**

---
# Parte MLFQ

## ¿Se puede producir starvation en el nuevo planificador?
Sí ­puede haber starvation, ya que los procesos que quedan en la cola de prioridades más baja puede que les lleve mucho más tiempo de lo que deberían en ejecutarse si hubiera muchos procesos en las colas de prioridades altas. En nuestro caso, los procesos pueden sufrir starvation, puesto que en ningún momento aplicamos la regla 5 del planificador MLFQ, que hace que los procesos suban a la prioridad más alta cada cierto tiempo. Los que más deberían sufrir de starvation serían los procesos cpubound ya que, con nuestra regla, cada vez que se desplanifique por un time interrupt, su prioridad será disminuida. Los procesos iobound tendrán la prioridad más alta al bloquearse más seguido antes de un time interrupt, por lo que mientras más procesos iobound haya, los procesos cpubound tardarán más en ejecutarse.

## Anotaciones acerca de la cuarta parte:
La modificación para la cuarta parte se hace en el scheduler, el cual, básicamente, recorre el arreglo de procesos en busca del próximo proceso a ejecutar segun las nuevas reglas.
Una vez hecha la implementación, vimos que los procesos cpubound seguían terminando todos antes que los procesos de iobound, lo que pensabamos que era incorrecto dado que los iobound tienen prioridad más alta que los cpubound, pero despues de varias pruebas, modificaciones al codigo, preguntas e investigaciones llegamos a la conclusión de que ese comportamiento no es incorrecto, el planificador funciona correctamente ya que durante la ejecución de todos los procesos, los iobound se planifican con mayor frecuencia, por lo que podemos concluir que efectivamente se cumple la regla de elegir al proceso que tiene mayor prioridad para ejecutar.

## Diferencias con Round Robin:
Las principales diferencias que pudimos observar con el quantum de 100.000 fueron que con el planificador mlfq, los iobound se planifican con una frecuencia distinta (en comparación con el Round Robin) dependiendo de la cantidad de procesos cpubound con los que se ejecute en paralelo. Por los experimentos realizados con el quantum de 100.000 podemos observar que mientras mayor sea la cantidad de procesos cpubound en comparación con la cantidad de procesos iobound, mayor será la frecuencia de planificación de los procesos iobound. En cambio, si la cantidad de procesos cpubound es mas o menos igual que la cantidad de procesos iobound, la frecuencia de planificación de ambos tipos de procesos se vuelve mas errática, puede variar desde que la cantidad de planificaciones sea muy similar, como se puede ver en el caso de ejecutar "iobench 10 &; cpubench 10 &", o puede tener un comportamiento tan aleatorio como tener procesos cpubench que tengan menor cantidad de planificaciones que un iobench y otros que tengan una cantidad mayor; otro caso posible es que haya mucho mas procesos iobench que cpubench, en el cual nos enfrentaríamos con la situación de que el cpubench (en varias ocasiones) sea planificado más veces que los iobenchs. 

---
