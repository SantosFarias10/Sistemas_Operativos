# Resumen

Este resumen esta pensando para entender Sistemas Operativos y poder tener las herramientas necesarias para el Final.

## Virtualizacion de la CPU

### Capitulo 2: Introduccion al SO

El SO (Sistema Operativo) es el **Software** que se encarga de que sea facil correr programas de forma eficiente y correcta. Para ello utiliza la **Virtualizacion**; una tecnica para representar al hardware de una forma simple, segura y eficiente, gracias a la  cual los programas pueden correr en simultaneo compartiendo CPU (Procesador), memoria (RAM) y dispositivos (Discos).

La interaccion entre el SO y los usuarios se da mediante una interfaz **API** (una **Standard Library**) que provee **System Calls** (Llamadas al Sistema) a los programas.

Como muchos programas corren concurrentemente, el SO tambien se encarga de ser el **Administrador** de los **Recursos**, entregando el uso del hardware real a los programas de forma eficiente y siguiendo diferentes **Mecanismos** y **Politicas**.

#### Virtualizacion de la CPU

El SO crea la ilusion de que hay muchos CPUs disponibles para que muchos programas puedan correr en simultaneo. La ejecucion de estos es manejada por diferentes **Mecanismos** y **Politicas** establecidas por el SO.

#### Virtualizacion de la Memoria

La **Memoria Fisica** no es mas que un `array` de bytes, y para poder leer o escribir en ella se necesita especificar una Direccion** en donde hacerlo.

Los programas acceden a memoria todo el tiempo, ya sea por informacion (archivos) o porque su propio codigo se encuentra alli (en cada instruccion `fetch` se accede a memoria).

Al ser **Virtualizada**, cada programa tiene su propio **Espacio Direccionable** de **Memoria Virtual** (**Address Space**). Esas direcciones no se corresponden con las reales de la memoria fisica (y por lo tanto, pueden repetirse entre programas), y es el SO quien se encarga de luego **Mapear** (traducir) esas direcciones de memoria virtual.

Los accesos a memoria de un programa no afectan (o no deberian xd, por proteccion) a otros programas o al SO mismo (eso es, aislamiento de procesos).

#### Concurrencia

Los problemas de concurrencia surgen con la ejecucion de programas en simultaneo (compartiendo memoria y CPU) o por programas que usan **Multiples Hilos**. Requiere de la ejecucion **Atomica** de instruccioness (toda la ejecucion de una sola vez, sin interferencias) para evitar comportamientos indefinidos y problemas de seguridad. Con el fin de evitar esas **Race Condition** (o posibles **Deadlocks**), se utilizan **Locks**, **Semaforos**.

#### Persistencia

La informacion en RAM es **Volatil** (su contenido se pierde al interrumpir el suministro de electricidad), por lo que el SO debe encargarse de la persistencia de la informacion. Lo hace a traves de hardware (con dispositivos de **I/O**, como discos) y el software (**Sistema de Archivos**).

Los dispositivos de almacenamiento no son virtualizados para cada programa, si no que estos pueden compartir informacion entre ellos. Poseen una velocidad muchos menor a la de la memoria RAM, pero son mas baratos y con una capacidad mayor.

----

### Capitulo 4: La Abstraccion de los Procesos

Un **Proceso** es la abstraccion del SO de un programa en ejecucion; el programa es un objeto estatico (codigo), mientras que el proceso es un objeto dinamico (corriendo en memoria).

La tecnica de ir intercalando varios procesos concurrentes, ejecutando uno a la vez, es conocida como **Time Sharing** (Tiempo Compartido), consiste en dividir un recurso (en el espacio) entre proceos que deseen utilizarlo.

#### Virtualizacion

Para lograr la ilusion de que hay muchas CPU (y, a su vez, que los procesos no tengan el control directo de lo que se ejecuta en la CPU fisica), el SO usa **Mecanismos** de bajo nivel; protocolos para implementar distintas funcionalidades. Algunas de ellas son el **Context Switch** (cambios de contextos), que permite cambiar un proceso en ejecucion por otro, y las diferentes **Politicas** de los **Planificadores**: algoritmos que toman decisiones sobre la distribucion de los recursos limitados en base a distintos factores y prioridades.

#### Procesos

Un proceso puede ser descrito (y por lo tanto guardado) por su **Estado**:

1. **Memoria**: Las instrucciones y la informacion que el proceso lee estan en memoria. La memoria a la que un proceso puede acceder (**Address Space**) es parte del proceso mismo.

2. **Registros** de la CPU: El programa puede leer o actualizar registros (como el **Program Counter** (**PC**), el **Stack Pointer**, o el **Frame Pointer**).

3. **Almacenamiento**: Un proceso puede acceder al almacenamiento (dispositivos I/O) para asegurar la persistencia.

#### API de los Procesos

Cualquier interfaz de procesos de un SO debe poder:

1. **Crear** nuevos procesos.

2. **Destruir** procesos en caso de que no terminen por si mismos.

3. **Esperar** la finalizacion de un proceso.

4. Tener **Control**: otros tipos de control, como la suspencion de un proceso.

5. Conocer su **Estado**: Poder mostrar informacion de un proceso.

Los programas utilizan estas funciones mediante **System Calls** proporcionadas por la API.

#### Creacion de un Proceso

Para correr un programa (osea convertirlo en proceso) el SO debe **Cargar** su codigo y su informacion estatica (con un formato ejecutable) del disco a la memoria, en el Address Space del proceso.

Se deben proporcionar memoria **Stack** (variables locales, parametros de llamada, direcciones de retorno, etc)y **Heap** (informacion dinamica y variables en tamaño, estructura de datos como listas; todo lo relacionado con `malloc`y `free`) para el programa. En el Stack, ademas, el SO establece los parametros `argv` (arreglos de cadenas (de tipo `char *`) que contiene los argumenos como `strings`) y `argc count` (entero que indica **Cuantos** argumentos se pasaron al programa desde la linea de comandos. Siempre es al menos 1, ya que se incluye el nombre del programa).

#### Estado de un Proceso

Cada proceso esta en alguno (solo uno) de los siguientes estados:

* **Running**: En ejecucion.

* **Ready**: En espera, listo para ejecutarse.

* **Blocked**: No puede correr hasta que otro evento ocurra (por ejemplo un dispositivo I/O).

* **Zombie**: El proceso hijo finaliza, pero el padre aun no ha llamado a `wait()` para leerlo.

El paso entre los distintos estados esta dado por eventos del software o hardware, llamadas **Interrupciones** (por ejemplo de reloj, del disco duro, etc).

A lo sumo puede haber `n` procesos en estado de Running, siendo `n` = cantidad de cores (nucleos).

#### Estructura de datos del SO

Listas de procesos (**Process List**): Contiene el listado de los procesos listos para correr o corriendo.

**PCB** (*Process Control Block*): Estructura de datos que, en cada entrada de sus tuplas, almacena el **Contexto de los Registros**, el mapa de la memoria, el estado del proceso, su *Process id* (**PID**), los archivos abiertos, el directorio actual (**PWD**: *Process Working Directory*), y el puntero al proceso padre. Ademas, almacena los datos de bajo nivel manejados por el procesador: el **Trap Frame** y el **Kernel Stack**.

Esta informacion sobre los procesos del sistema nos permite realizar un **Context Switch**: "congelarlos", guardar o colocar la informacion almacenada en los registros, y continuar la ejecucion de un proceso. El SO almacena un array de PCBs.

* **User Time**: Tiempo de computo del proceso en modo usuario.

* **CPU Time**: Tiempo total que un proceso ha usado la CPU. Si ejecuta mas de un hilo, es la suma del tiempo de todos ellos.

* **Wall Time**: Tiempo total real transcurrido, desde que comienza a ejecutarse un proceso hasta que termina.

#### Notar

* &WallTime >= CPU Time$: Cuando el proceso se ejecuta en un solo nucleo o en multiples nucleos pero sin paralelismo (un nucleo alternado entre varios hilos).

* $WallTime < CPU Time$: Puede llegar a suceder si el proceso se ejecuta en multiples nucleos simultaneamente (paralelismo), ya que el CPU time se suma en cada nucleo que esta en uso.

* $UserTime < WallTime$: El proceso parte del tiempo esperando: ya sea I/O, bloqueado, o al `return` del trap cuando hace una System Call y pasa a modo kernel.

* $UserTime = WallTime$: Un proceso sin hilos se ejecuta en modo usuario, sin interrupciones ni operaciones de I/O, sin llamadas al sistema, y no se bloquea ni o sufre Context Switches.

* $UserTime > WallTime$: Proceso multihilo, en el que cada hilo ejecuta en un core distinto y luego se suman todos sus tiempos de usuario.

---

### Capitulo 5: La API de los Procesos

Interfaz que permite interaccionar con la abstraccion de los procesos.

La creacion de procesos en Unix se da con dos System Calls: `fork()` y `exec()`.

Puede usarse `wait()` para esperar que un proceso creado termine. 

Cada proceso tiene un identificador unico, un numero llado **PID**.

* `argc`: Entero que representa el numero de argumentos pasados a la linea de comandos. Incluye el nombre del programa como el primer argumento, por lo que `argc` siempre es al menos 1.

* `argv[]`: Array de punteros a cadenas de caracteres que contiene el nombre del programa a ejecutar (en `argv[0]`) y los argumentos pasados al programa en el resto de posiciones. Siempre esta terminado po un puntero `null` (`argv[argc]` = `null`).

#### `fork()` System Call

Crea un nuevo proceso. El proceso creado es una **Copia** casi identica del proceso donde fue llamado `fork()` (**Parent**); el proceso creado (**Child**) tiene una copia del **Address Space** (mapa de memoria) pero es su propia memoria privada, sus propios **Registros** pero con el mismo contenido, el mismo **PC**, etc.

Tiene aridad (numero de argumentos) 0 (No toma argumentos) y devuelve un entero; 0 para el proceso Child, y el PID del Child para el proceso padre (o un numero negativo si falla). Sus ejecuciones no son deterministas; son dos procesos diferentes y el *Scheduler* va a determinar cual ejecuta en cada momento.

#### `exec()` System Call

Es una familia de System Call (por ejemplo `execv`) que se utilizan para correr un programa diferente al programa desde el cual se la llama. Usa el nombre de un programa como argumento (`argv`), carga el **Ejecutable**, y sobreescribe el segmento de codigo actual. Luego, el SO corre ese programa.

* `execv()`: El primer argumento es el *path* al programa, el segundo argumento es un array de punteros a los argumentos, terminado en `null`.

* `execvp()`: El primer argumento es el nombre *file* del programa (se busca en el *path* actual), el segundo argumento es un array de punteros a los argumentos.

* `execl()`: se pasan los argumentos como una lista explicita en la llamada, uno por uno, terminando con `null`.

#### `wait()` System Call

Es usada por un proceso Parent para **Esperar** a que el proceso **Child** termine de ejecutarse. Recien en ese momento el Parent continua su ejecucion.

Si un Parent tiene multiples Childs, puede usar la version `waitpid()` para especificar el PID de un Child especifico al cual esperar (si no, el retorno del `wait` se vuelve no determinista).

La separacion de `fork()` y `exec()` (diferenciando la creacion de la ejecucion) da la oportunidad de ejecutar procesos de diversas formas y con varias funcionalidades en el medio, ya que permite cambiar las **Variables de Entorno** y los **Archivos Abiertos** (*File Descriptors*) en el espacio intermedio entre la ejecucion de un proceso y otro.

Por ejemplo: Permite utilizar una pipe (|) en shell para redireccionar (usando `open`, `close` y `dup` (duplicar el *file descriptor*)) la salida de un comando/proceso hacia el input del siguiente.

#### Control de Procesos y Usuarios

`kill()`: Familia de System Calls usadas para enviar distintas señales (**Signals**) asincronas a un proceso, por ejemplo: `SIGSTOP`, `SIGKILL`, `SIGCONT`, etc.

`signal()` es usada por los procesos para hacer *catch* de sañes enviadas al mismo. Esta es una forma primitiva de comunicacion entre los procesos.

Con la nocion de usuario se limita quien gana control sobre los recursos del sistema y quien puede controlar todos los procesos, o solo los propios (por razones de seguridad).

---

### Capitulo 6: Ejecucion Directa Limitada (LDE)
