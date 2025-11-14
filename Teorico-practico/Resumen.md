# Introduccion

## Resumen de Santos, miembro de Wolovers

Basado en el [Resumen](https://docs.google.com/document/d/1e5x10V286rX_EuQmJoCVOha-DTRf_hp-CFGZURr5f5Q/edit?tab=t.0) de Navier🧃, en las [Traducciones](https://github.com/GabrielHak/SistemasOperativos) de GabrielHak, y el [OSTEP](https://pages.cs.wisc.edu/~remzi/OSTEP/), mas los [Podcasts](https://open.spotify.com/show/5glisR9k3xqeMLwhDM92KZ?si=8fac02c5c1ea49e2) y notas tomadas en clases del Dios Nicolas Wolovick.

Intente en reestructurar un poco el resumen en el que esta basado, rellenar algunos capitulos que me parecia que le faltaba informacion y contexto.

Por ultimo, este resumen NO te garantiza que apruebes 100%, pero si te ayudara un 90% en entender todos los conceptos, para mas informacion dejo mis [Soluciones de Parciales]().

Dios los bendiga 🚬

## Indice

### [Virtualizacion de la CPU](#virtualizacion-de-la-cpu)
- [Capitulo 4: La Abstraccion de los Procesos](#capitulo-4-la-abstraccion-de-los-procesos)
- [Capitulo 5: La Api de los Procesos](#capitulo-5-la-api-de-los-procesos)
- [Capitulo 6: Ejecucion Directa Limidata (LDE)](#capitulo-6-ejecucion-directa-limidata-lde)
- [Capitulo 7: Planificacion de la CPU](#capitulo-7-planificacion-de-la-cpu)
- [Capitulo 8: Cola MultiNivel con Retroalimentacion (MLFQ)](#capitulo-8-cola-multinivel-con-retroalimentacion-mlfq)

### [Virtualizacion de la Memoria](#virtualizacion-de-la-memoria)
- [Capitulo 13: El Espacio de Direcciones](#capitulo-13-el-espacio-de-direcciones)

# Virtualizacion de la CPU

## Capitulo 4: La Abstraccion de los Procesos

Un **Proceso** es la abstraccion del SO de un programa en ejecucion; el **programa es un objeto estatico** (*codigo*), mientras que el **proceso es un objeto dinamico** (el programa mientras se esta ejecutando en memoria).
<br>El SO usa la tecnica de ***Time Sharing*** (**Tiempo Compartido**) de la CPU para permitir a los usuarios ejecutar tantos procesos concurrentes como deseen; la tecnica consiste en ejecutar un proceso, luego deteniendolo y ejecutando otro, y asi sucesivamente.

Por otro lado, el **Space Sharing** (**Espacio Compartido**) que consiste en dividir un recurso (en el espacio) entre los proceso que deseen utilizarlo.

### Virtualizacion

Para lograr la ilusion de que hay muchas CPUs, el SO necesitara tanto maquinaria de bajo nivel como inteligencia de alto nivel. La maquinaria de bajo nivel la llamamos **Mecanismos**; metodos o protocolos de bajo nivel que implementan una parte de la funcionalidad necesaria. Por ejemplo: El ***Context Switch*** (**Cambio de Contexto**), que permite cambiar un proceso en ejecucion por otro.
<br>Encima de estos mecanismos esta la parte de la inteligencia del SO, en forma de **Politicas**; algoritmos para tomar algun tipo de decision dentro del SO. Por ejemplo: Dado un numero de programas posibles para ejecutar en una CPU, ¿Que programa deberia ejecutar el SO? La **Politica de Planificacion** es la que tomara esta decision.

### Proceso

Un proceso puede ser descrito por su **Estado**; lo que un programa puede leer o actualizar cuando se esta ejecutando.
* **Memoria**: Instrucciones y la informacion que el proceso lee y escribe estan en memoria. La memoria a la que un proceso puede acceder (***Addres Space***) es parte del proceso.
* **Registros**: El programa puede leer o actualizar registros (como el ***Program Counter*** (PC), que indica que instruccion se ejecutara a continuacion, el ***Stack Pointer***, o el ***Frame Pointer***).
* **Almacenamiento**: Un programa puede acceder al almacenamiento (dispositivos ***Input/Output*** (**I/O**)) para asegurar la persistencia.

### API de los Procesos

Cualquier interfaz de procesos de un SO debe poder:
* **Crear** nuevos procesos.
* **Destruir** procesos (en caso de que no terminen por si mismo).
* **Esperar** la finalizacion de un proceso.
* Tener **Control**: Otros tipos de control, como la suspencion de un proceso.
* Conocer su **Estado**: Poder mostrar informacion de un proceso.

Los programas utilizan estas funciones mediante System Calls Proporcionadas por la API.

### Creacion de un Proceso

Lo primero que hace el SO para correr un programa (osea, convertirlo en proceso) el SO debe **Cargar** su codigo y su informacion estatica (con un formato ejecutable) del Disco a la Memoria (Disco -> Memoria), en el Address Space del proceso.
<br>Luego se debe proporcionar memoria ***Stack*** (variables locales, parametros de llamada, direcciones de retorno) y ***Heap*** (informacion dinamica y variables en tamaño, estructura de datos como Listas; todo lo relacionado con `malloc` y `free`) para el programa. En el Stack el SO establece los parametros `argv` y `argc count`.
<br>Se inicializan los ***File Descriptors*** (`std in`, `std out`, `std error`)(0, 1, 2).
<br>Por ultimo , el SO setea todos los registros a 0 (menos el PC) y pasa el control de la CPU al proceso creado, dejandolo ejecutarse.

![Creacion de un Proceso](../Teorico-practico/imagenes/CreacionDeUnProceso.png)
* Memoria asignada a un proceso en el momento de su creacion.

### Estados de los Procesos

Cada proceso puede estar en uno de los siguientes 4 estados:
* ***Running***: En ejecucion.
* ***Ready***: En espera, listo para ejecutarse.
* ***Blocked***: No puede ejecutarse hasta que otro evento ocurra.
* ***Zombie***: El proceso hijo finaliza, pero el padre aun no ha llamado a `wait()`.

![Estados de los procesos](../Teorico-practico/imagenes/EstadosDeLosProcesos.png)
* Transiciones entre estados de un proceso.

El paso entre los distintos estados esta dado por eventos del software o hardware, llamadas **Interrupciones** (por ejemplo: de reloj, del disco duro, etc).
<br>A lo sumo, puede haber `n` procesos en estado de Running, siendo `n = cantidad de cores` (nucleos).

* Un proceso puede moverse entre los estados Ready y Running a discrecion del SO. 
* Pasar de Ready a Running significa que el proceso ha sido **Planificado** (***Scheduled***);
* Ser movido de Running a Ready significa que el proceso ha sido **Desplanificado** (***Descheduled***).
* Si un proceso ha sido bloqueado, el SO lo mantendra asi hasta que ocurra algun evento; el proceso vuelve al estado Ready.

### Estructura de Datos del SO

El So tiene algunas estructuras de datos que guardan informacion. Para guardar el estado de cada proceso, el SO tiene algun tipo de ***Process List*** (**Listas de Procesos**) que sirve como el registro de todos los procesos activos en el sistema. Esta lista contiene informacion de todos los procesos que estan Ready, asi como aquellos que estan bloqueados o en ejecucion.
<br>Cada entrada de la lista corresponde un ***Process Control Block*** (**PCB**) (**Bloque de Control de Proceso**); estructura que almacena toda la informacion relevante sobre un proceso especifico. Contiene, entre otros campos:
* ***Context*** (**Contexto**) de los registros: Cuando un proceso es detenido, el contenido de sus registros de CPU se guarda en esta estructura. Para reanudar el proceso, el SO simplemente restaura estos valores en los registros fisicos de la CPU.
* ***State*** (**Estado**) del proceso: Este campo indica el estado actual del proceso.
* ***Process Id*** (**PID**) (**Id del Proceso**): Identificador numerico unico para cada proceso.

### Times

* ***User Time***: Tiempo de **Computo** del proceso en modo usuario.
* ***CPU Time***: Tiempo total que un proceso ha usado la CPU. Si se ejecuta en mas de un hilo, es la suma del tiempo de todos ellos.
* ***Wall Time***: Tiempo total real transcurrido, desde que comienza a ejecutarse un proceso hasta que termina.

* ***WallTime ≥ CPUTime***
  * Cuando el proceso usa un solo núcleo, o varios sin paralelismo real (por ejemplo, hilos que se alternan en un mismo núcleo).
  * => El reloj sigue corriendo mientras la CPU se reparte entre tareas, por lo que el tiempo real suele ser mayor.
* ***CPUTime > WallTime***
  * Ocurre cuando el proceso se ejecuta en varios núcleos en paralelo.
  * => Cada núcleo aporta su propio tiempo de CPU, y al sumarlos el total puede superar al tiempo real transcurrido.
* ***WallTime > UserTime***
  * Normalmente sucede porque el proceso no siempre está ejecutando código en modo usuario; puede estar esperando I/O, bloqueado o en modo kernel durante una syscall.
* ***UserTime = WallTime***
  * Caso idealizado: un proceso de un solo hilo, que no realiza I/O ni llamadas al sistema, y se ejecuta continuamente en modo usuario sin interrupciones ni cambios de contexto.
* ***UserTime > WallTime***
  * Esto puede pasar en procesos multihilo que se ejecutan realmente en paralelo, ya que el User Time total se suma entre los hilos, mientras que el Wall Time mide solo el tiempo real del proceso completo.

## Capitulo 5: La Api de los Procesos

La creacion de procesos, en Unix, se da con dos ***System Calls*** (Llamadas al sistema): `fork()` y `exec()`. Puede usarse `wait()` para esperar que un proceso creado termine.
<br>Cada proceso tiene un identificador unico, un numero llamado **PID**.

Cuando un nuevo programa empieza a ejecutarse, el SO puede pasarle argumentos desde la linea de comandos.
<br>Esto se hace a traves de los parametros del `main()`

```c
int main(int argc, char *argv[])
```

* `argc`: Es un entero que representa el numero de argumentos pasados a la linea de comandos. Incluye el nombre del programa como el primer argumento, por lo que `argc` siempre es al menos 1.
* `argv[]`: Es un array de punteros a cadenas de caracteres que contiene el nombre del programa a ejecutar (en `argv[0]`) y los argumentos pasados al programa en el resto de posiciones. El ultimo elemento siempre es un puntero `NULL` (`argv[argc] = NULL`)

### System Call `fork()`

Crea un nuevo proceso. El proceso creado es una copia casi identica del proceso donde fue llamado `fork()` (***Parent***, **Padre**); el proceso creado (***Child***, **Hijo**) tiene una copia del ***Adress Space*** (Espacio de Direcciones, se puede pensar que es el "Mapa de Memoria" privado de un proceso, Es el conjunto de todas las direcciones de memoria que el proceso *cree* que tiene disponibles) pero con su propia memoria privada, sus propios **Registros** pero con el mismo contenido, el mismo **PC**, etc.
<br>Sus ejecuciones **NO son Deterministas**, es decir que no se puede predecir en que orden van a ejecutarse el proceso padre y el proceso hijo despues del `fork()`.
<br>No toma argumentos, se dice que tiene aridad 0 (aridad es el numero de argumentos) y devuelve:
* **0** para el proceso hijo.
* El **PID** del hijo para el proceso padre.
* Y un **Numero Negativo** en el caso de que falle.

Cada vez que se llama a `fork()`, el proceso actual se duplica.
<br>Por eso, si dentro de un programa hay varias llamadas a `fork()`, la cantidad total de procesos crece exponencialmente.
<br>En general con `n` llamadas a `fork()`, se crean **$2^{n}$** procesos en total.

### System Call `exec()`

Se utiliza para reemplazar el programa actual por otro nuevo. El proceso que llama a `exec(const char *path, char *const argv[])` no crea un hijo, si no que cambia su propio codigo y datos por los del nuevo programa. Usa el nombre de un programa como argumento (`argv`), carga el **Ejecutable**, y sobreescribe el codigo actual. Luego, el SO corre ese programa.
* `execv(path, argv[])`: Ejecuta el programa ubicado en `path`, pasando los argumentos en un array terminado en `NULL`.
* `execvp(file, argv[])`: Es igual que el `execv`, pero busca el programa en el PATH del sistema.
* `execl(path, arg1, arg2, ..., NULL)`: Los argumentos se pasan uno por uno en la llamada.

### System Call `wait()`

Es usada por un proceso padre para **Esperar** a que el proceso hijo termine de ejecutarse. Recien en ese momento el padre continua su ejecucion.
<br> Si un padre tiene multiples hijos, puede usar la version `waitpid()` para especificar el PID de un hijo especifico al cual esperar.

La separacion de `fork()` y `exec()` (diferenciando la creacion de la ejecucion) da la oportunidad de ejecutar procesos de diversas formas y con varias funcionalidades en el medio.
<br> Entre ambas llamadas, el proceso puede modificar su entorno antes de ejecutar el nuevo programa, por ejemplo:
* Cambiar **Variables de Entorno**.
* Ajustar o redirigir Archivos Abiertos mediante sus *File Descriptors*.

Esto hace posible funcionalidades como el uso de *pipe* ( | ) en la shell, donde la salida de un proceso se redirecciona como entrada del siguiente usando llamadas como `open()`, `close()` y `dup()` (que duplica un *File Descriptor*).

### Control de Procesos y Usuarios

Los procesos pueden **Enviar y Recibir Señales** (***Signals***), que son mensajes asincronos usados para controlar su ejecucion o comunicarse entre si.
* `kill()`: Se usa para mandar ***signals*** a un proceso, incluyendo directivas para pausar, matar y otros imperativos utiles.
* `signal()`: Se usa en un proceso para "agarrar" varias *signals*.

## Capitulo 6: Ejecucion Directa Limidata (LDE)

Para virtualizar la CPU el SO necesita compartir la CPU fisica entre varios trabajos simultaneos. La idea principal es ejecutar uno un poco y cambiar a otro rapidamente generando la ilusion de procesamiento simultaneo. Con ***Time Sharing*** (**Tiempo Compartido**) se alcanza la virtualizacion.
<br>Ademas, debe lograrse asegurando la ***Perfomance*** (**Rendimiento**) (virtualizar sin sobrecargar el sistema) y el **Control** (como correr cada proceso mientras mantenemos control del CPU).

### Ejecucion Directa Limitada

La **Ejecucion Directa Limitada** consiste en correr los programas directamente en la CPU; el SO hace los preparativos:
* Crea una nueva entrada para el proceso en la *Tabla de Procesos*, o sea crea un nuevo PCB (Bloque de Control de Proceso),
* Reserva memoria para el programa,
* Carga desde el sistema de arhivos el codigo ejecutable a la memoria,
* Pone el `argv` y `argc` en la Pila (*Stack*),
* Limpia los registros,
* Y llama a `main`.
* Y por ultimo lo ejecuta.

### Operaciones Restringidas

Una de las ventajas del metodo de Ejecucion Directa Limitada es su Ejecucion Rapida, pero debe controlarse que el proceso no haga cosas que no queremos que haga sin el apoyo del SO. La solucion que usamos es introducir un nuevo modo de procesador, conocida como ***User Mode*** (**Modo Usuario**).
<br>En *User Mode* (restringido) el codigo corre con restricciones (como acceso al I/O a memoria no permitida).
<br>En ***Kernel Mode*** (privilegiado), modo en el que funciona el SO, el proceso no tiene restricciones y puede hacer operaciones privilegiadas.

El *User Mode* cuenta con *System Calls* para solicitar operaciones que tiene restringidas. Estas instrucciones ejecutan una ***Trap*** que salta a *kernel mode* elevando el privilegio y realizando la instruccion (si es que el SO lo permite). Al terminar, se realizan un ***Return From Trap*** y baja el nivel de privilegio volviendo a *user mode*.
<br>Antes de ejecutar una *trap*, se guardan los registros (contexto) del proceso que llamo a la *trap*, en un ***Kernel Stack*** (uno por proceso), para poder restaurarlos al volver al *user mode*. El kernel necesita saber que rutina ejecutar ante cada tipo de excepcion o interrupcion. Para eso, durante el arranque del sistema (*booteo*), el SO configura una ***Trap Table***; tabla que asocia cada tipo de evento con su correspondiente ***Trap Handler*** (la funcion del kernel que debe manejarlo). Estos *handlers* son ejecutados por el SO en *kernel mode*.

Esta indireccion mejora la seguridad (el usuaria no ejecuta codigo del kernel directamente) y permite reemplazar o modificar el kernel sin afectar a los programas, siempre que los IDs de las *system calls* se mantengan iguales.

Una *trap* puede ser ocasionada por:
* Un ***Timer Interrupt*** (evento asincrono).
* Un *hardware device interrupt* (evento asincrono).
* Una *Syscall* (evento sincrono).
* Una *exception* (errores, evento sincrono).

El SO reacciona a cualquiera de ellas de la misma forma (trap handle)

Cada *system call* tiene una ***System-call Number***. Cuando un proceso se invoca, este numero se envia al kernel, que en el *trap handler* verifica que llamada es y ejecuta la funcion correspondiente si es valida. Esto ofrece proteccion para no darle control total al usuario, pero no controla los argumentos que el usuario pasa a las *system calls*. Si el kernel no valida ese *input*, puede causar errores o vulnerabilidades.

En LDE, durante el *booteo*, el SO configura una *trap table* que le indica al CPU como responder ante interrupciones o *system calls*. Cuando el kernel lanza un proceso, le asigna memoria y cambia el CPU a *user mode* para ejecutarlo. Si el proceso necesita usar el SO, realiza una *trap* que lo devuelva temporalmente a *kernel mode*, el SO maneja la llamada, y luego retorna al *user mode* para continuar.

Cada proceso tiene un *kernel stack*, donde los registros (incluyendo el PC) se guardan y restauran  (por hardware) cuando se entra o sale del kernel.

![Linea de tiempo](../Teorico-practico/imagenes/lineaDelTiempoDeLasDosFasesDelProtocoloDeLDEAnteUnaTrapPorInstrucciónPrivilegiada.png)
* Linea del tiempo de las dos fases del protocolo de LDE ante una trap por instruccion privilegiada.

### Intercambio Entre Procesos

Si un proceso corre en el CPU, el SO no esta corriendo. Sin embargo, el mismo debe asegurarse de poder recuperar el control para cambiar de proceso y lograr *Time Sharing*.

#### Enfoque Colaborativo: Esperar a Nuevas *System Calls*
El SO confia en el proceso en ejecucion y da por sentado que cada cierto periodo de tiempo liberaria el CPU para que el SO decida correr; al ejecutarse una *trap* o al llamar a una *system call* (incluso de manera explicita, con la llamada `yield`) se cederia y transferiria el control al SO.

#### Enfoque No Colaborativo: El SO Toma el Control
Utilizando mecanismos del hardware, el SO puede retomar el control del CPU mediante el ***Timer Interrupt***; un dispositivo que cada varios milisegundos realiza una interrupcion. Esta cuando ocurre se ejecuta un ***Interrupt Handle*** en el SO, que le permite retomar el control.
<br>En el *booteo* el SO deja explicito que codigo correr en una interrupcion, momento en el que ademas comienza el *timer*.
<br>El hardware se encarga de guardar el estado del proceso actual al momento de la interrupcion para su posterior *return from trap*.

![ViteComoE](../Teorico-practico/imagenes/dibujitoAMano.png)
* En cada proceso una porcion se ejecuta en *user mode* y otra en *kernel mode*. Para pasar al *scheduler*, se da un segundo cambio de contexto; de *kernel mode* al *scheduler*. Por lo que tenemos: *user mode* -> *kernel mode* -> *scheduler*
* En el cambio de contexto de *user mode* a *kernel mode*, debemos guardar todo el contexto del proceso para poder retomarlo despues. En cambio, cuando se pasa del *kernel mode* al *scheduler*, ya se esta dentro el SO, por lo que no hace falta guardar tanta informacion: El sistema asumir ciertas partes del entorno son seguras y controladas.

### Guardar y Restaurar el Contexto

Si en el momento que el SO toma el control y decide cambiar a otro proceso (usando la rutina *switch*), ejecuta un codigo de bajo nivel, el ***context switch***, que le permite guardar los valores de los registros del programa en ejecucion (en el *kernel stack* del proceso) y restaurar otro para el proceso que pasara a ejecutarse (desde su *kernel stack*).

Si hay un *timer interrupt* (*trap*), es el hardware quien guarda los registros en el *stack* del kernel. Si hay un *switch* por parte del SO, es el software quien guarda/restaura los registros del kernel en la estructura del proceso.

Cada vez que se da un *context switch* (por ejemplo, ante una *syscall exception*), el proceso pierde *perfomance*, y tarda determinados ciclos en retomar la velocidad que tenia antes de la mismas.

### Concurrencia

Para que no ocurran *interrupts* simulteneas, se suelen deshabilitar las interrupciones mientras se esta lidiando con interrupciones. Tambien se usan ***locks*** para proteger las estructuras internas.

![Linea de tiempo](../Teorico-practico/imagenes/LineaDelTiempo2DeLaSegundaFaseDelProtocoloDeLDEAnteUnaInterrupciónPorTiempo.png)
* Linea del tiempo de la segunda fase del protocolo de LDE ante una interrupcion por tiempo.

## Capitulo 7: Planificacion de la CPU

Las politicas de alto nivel (***Scheduling Policies***, o ***Diciplines***) son algoritmos utilizados por el *scheduler* del SO para decidir que procesos ejecutar y en que orden.

### Suposiciones Sobre la Carga de Trabajo

Antes de ver como se desarrolla una politica, hacemos alguna suposiciones sobre los procesos que corren en el sistema (la carga de trabajo, o *workload*), las cuales no son realistas:
1. Todos los procesos duran la misma cantidad de tiempo hasta acabar.
2. Todos los procesos llegan al mismo tiempo.
3. Una vez empiezan, se ejecutan hasta acabar.
4. Todos los procesos solo usan la CPU (o sea, no hacen I/O).
5. El tiempo de ejecucion de cada proceso es conocido

### Metricas de la Planificacion

El ***Turnaround Time*** ($T_{turnaround}$) (**Tiempo de Entrega**) es la metrica que mide el tiempo total que tarda un proceso desde que llega al sistema hasta que finaliza:

$$
T_{turnaround} = T_{completion} - T_{arrival}
$$

O sea, Tiempo de entrega = Tiempo de finalizacion - Tiempo de llegada.

Si se cumple la suposicion (2) => $T_{arrival} = 0$, lo que hace que $T_{turnaround} = T_{completion}$.

*Turnaround time* es una metrica de *perfomance*.

### Politica: FIFO (*Firsr In, First Out*) o FCFS (*First Come, Firse Serve*)

Es simple y facil de implementar pero con un mal tiempo de entrega. Como su nombre indica el primer proceso en entrar, es el proceso que se va a ejecutar hasta su finalizacion.
<br>Ejemplo: Supongamos que llegan tres procesos, `A`, `B` y `C` aproximadamente al mismo tiempo (Tiempo de llegada = 0). FIFO ubica algun proceso primero, es por eso que asumimos que `A` llega antes que `B` y `B` llega antes que `C`. Supongamos que cada proceso se ejecuta por 10 segundos.

![](../Teorico-practico/imagenes/FIFOBien.png)

Vemos que `A` termina al segundo 10, `B` al 20 y `C` al 30. Por lo que el Tiempo de Entrega (Turnaround time) para los tres procesos es: $\frac{10+20+30}{3} = 20$.
<br>Pero no todos los procesos duran la misma cantidad de tiempo (suposicion (1)).
<br>Ejemplo: Supongamos que llegan 3 procesos (`A`, `B` y `C`), pero esta vez `A` se ejecuta por 100 segundos, y `B` y `C` se ejecutan 10 segundos cada uno.

![](../Teorico-practico/imagenes/FIFOMal.png)

Vemos que el proceso `A` se ejecuta primero los 100 segundos completos antes de que `B` y `C`. Por lo que el tiempo de entrega promedio es alto: $\frac{100+110+120}{3} = 110$.
<br>Este problema es conocido como ***Convoy Effect***, o sea, puede llegar antes un proceso mas largo que los demas y relentizar al resto, teniendo procesos mas cortos en espera, aumentando asi el tiempo promedio de entrega del sistema.

### Politica: SJF (*Shortest Job First*)

Este enfoque simple resuelve el problema, ejecuta el proceso mas corto primero, y luego el siguiente mas corto, y asi sucesivamente.
<br> Tomamos el ejemplo anterior pero esta vez con SJF como politica de planificacion.

![](../Teorico-practico/imagenes/SJFBien.png)

El diagrama muestra como porque SJF se desempeña mejor con respecto al tiempo de entrega promedio. Simplemente ejecutando `B` y `C` antes que `A`, SJF reduce el tiempo promedio de 110 a $\frac{10+20+120}{3} = 50$ segundos, menos de la mitad del tiempo.
<br>Pero, no todos los procesos llegan al mismo tiempo (suposicion 2).
<br>Ejemplo: Supongamos que `A` llega en $t = 0$ y necesita ejecutarse por 100 segundos, mientras que `B` y `C` llegan en $t = 10$ y cada uno necesita ejecutarse por 10 segundos. Con SJF obtendremos:

![](../Teorico-practico/imagenes/SJFMal.png)

A pesar de que `B` y `C` llegaron poco despues que `A`, ellos son forzados a esperar a que `A` termine su ejecucion, y por lo tanto sufre el mismo *convoy effect*. El tiempo de entrega promedio para estos tres procesos es de $\frac{100+(110-10)+(120-10)}{3} = 103,33$

### Politica: STCF (*Shortest Time-to-Completion First*) o PSJF (*Preemptive Shortest Job First*)

Para esta politica suavizamos/olvidamos la suposicion (3) los Procesos no se ejecutan hasta terminarse. Tambien necesitamos algo de maquinaria en el planificador. Tomando el ejemplo, el planificador puede hacer algo cuando `B` y `C` llegan; puede reemplazar (*preempt*) y decidir ejecutar otro proceso, y quizas continuar ejecutando `A` despues.
<br>Este planificador agrega reemplazo a SJF. Cuando entra un nuevo proceso al sistema, el planificador STCF determina a cual de los procesos que quedan le queda el menor tiempo de ejecucion, y planifica ese. Por l tanto, en nuestro ejemplo, STCF reemplazara `A` y ejecutara `B` y `C` hasta completarse; solo entonces podra planificarse el tiempo que le queda a `A`.

![](../Teorico-practico/imagenes/STCF.png)

El tiempo de entrega promedio mejoro con respecto al JSF: $\frac{(120-0)+(20-10)+(30-10)}{3} = 50$.

### Metrica: *Response Time*

Las computadoras actuales deben tener un *perfomance* interactiva con el usuario, y esto es lo que se mide en ***Response Time*** (**Tiempo de Respuesta**). Definimos el tiempo de respuesta como el tiempo desde que el proceso llego al sistema hasta el tiempo en que fue planificado por primera vez. Formalmente quedaria:

$$
T_{response} = T_{firstrun} - T_{arrival}
$$

O sea, el Tiempo de Respuesta = Primera Ejecucion - Momento de llegada

Por ejemplo, si tenemos la planificacion del caso anterior (Con `A` llegando al 0, y `B` y `C` al 10), el tiempo de respuesta de cada proceso seria: 0 para `A`, 0 para `B` y 10 para `C` (promedio 3,33).
<br>STCF y las disciplinas relacionadas No son buenas para el tiempo de respuesta.

### Planificacion: *Round Robin* (**RR**)

Corre los procesos durante un periodo de tiempo fijo o hasta que acaben (este segmente, ***Time Slice***, es llamado ***Quantum***). Al ser cortado por un Q, el proceso va al final de una FIFO. Cambia de proceso cada cierto tiempo fijo de entre los trabajos que hay en una cola hasta que finalicen. este *time slice* debe ser multiplo del *timer interrupt* del sistema para que el SO pueda tomar control y hacer el *context switch* en ese momento.
<br>Ejemplo: Supongamos que hay tres procesos `A`, `B` y `C` llegan al mismo tiempo al sistema, y que cada uno quiere ejecutarse 5 segundos. Un planificador SJF ejecutara cada proceso hasta que se complete antes de ejecutar otro, como muestra la siguiente imagen:

![](../Teorico-practico/imagenes/SJFRR.png)

A diferencia de RR con un *time slice* de 1 segundo que ciclara a traves de los procesos rapidamente:

![](../Teorico-practico/imagenes/RRTimeSlice1s.png)

El tiempo de respuesta promedio de RR es:

$$
\frac{0+1+2}{3} = 1
$$

<br>Y el tiempo de respuesta promedio de SJF es:

$$
\frac{0+5+10}{3} = 5
$$

La duracion del *time slice* es importante; si es muy corta es bueno para el Tiempo de respuesta pero puede empeorar la *perfomance* al aumentar la cantidad de *context switchs*. A la vez, debe ser suficientemente larga para amortizar el costo del cambio y no empeorar la *perfomance*, y corta para mantener un tiempo de respuesta aceptable.

RR es ***Fair*** (**Justa**), lo que hace que sea mala en su tiempo de entrega. Es un *trade-off* a considerar dependiendo del objetivo que se tenga con el *scheduler*.

Ante dos procesos que arriban a la vez se debe establecer una politica que determine cual se ejecuta, pero si uno ya estaba *ready* antes que otro, se respeta el orden FIFO, no el de la politica.

### Incorporando I/O

Obviamente todos los programas hacen I/O (suposicion (4)). Un planificador tiene una decision que tomar cuando un proceso inicia una peticion de I/O; porque el proceso que se esta ejecutando actualmente no puede usar la CPU durante la I/O; se bloquea esperando que se complete la I/O. Si la I/O es enviada al disco duro, el proceso sera bloqueado durante algunos milisegundos. Por lo tanto el planificador deberia planificar otro proceso en la CPU en ese tiempo.
<br>El planificador tambien debera tomar una decision cuando una I/O se completa. Cuando esto ocurre, se genera una interrupcion, y el SO cambia el estado *blocked* del proceso que hizo la peticion a *ready*. Por supuesto, todavia debera decidir ejecutar el proceso o no.

Ejemplo: Asumimos que hay dos proceso `A` y `B`, los cuales necesitan 50 milisegundos de tiempo de CPU. Sim embargo, hay una diferencia: `A` se ejecuta por 10 milisegundos y emite una peticion I/O (asumimos que cada I/O tarda 10 milisegundos), cuando `B` se ejecuta por 50 milisegundos y no hace ninguna I/O. El planificador ejecuta primero `A` y luego `B`.

![](../Teorico-practico/imagenes/I_O.png)

Ahora asumamos que estamos tratando de construir un planificador STCF. ¿Como explicaria un planificador de este tipo el hecho de que `A` se divide en 5 subtrabajos de 10 milisegundos, cuando `B` solo demanda 50 milisegundos de CPU? Ejecutar un proceso y recien despues otro sin tener en cuenta las I/O no tiene sentido. Un enfoque comun es tratar cada sub proceso de 10 milisegundos de `A` como un proceso independiente. Por lo que, cuando el sistema empieza, su opcion seria, ya sea ejecutar 10 milisegundos `A` o ejecutar 50 milisegundos `B`. Con STCF, La opcion es clara: elegir la mas corta, en este caso `A`. Entonces, cuando el primer subproceso de `A` se completa, solo queda `B` y empieza a ejecutarse. Entonces un nuevo subproceso de `A` es mandado y reemplaza a `B` y se ejecuta por 10 milisegundos. Hacer esto permite la **Superposicion** (***Overlap***), con la CPU siendo usada por otro proceso mientras espera a que el I/O de otro proceso se complete; por lo tanto el sistema es mejor utilizado (de forma mas eficiente).

![](../Teorico-practico/imagenes/I_O_Overlap.png)

Asi es como un planificador incorpora I/O. Tratando cada uso de CPU como un proceso diferente, el planificador se asegura que los procesos son "interactivos" ejecutandolos frecuentemente. Mientras estos procesos intereactivos estan haciendo I/O, otros procesos que usan la CPU se ejecutan, por lo tanto es una mejor utilizacion del procesador.

### No Mas Oraculos

Nuestra suposicion final: que el planificador conoce el tiempo de ejecucion de cada proceso. Es la peor suposicion que se pudo hacer, ya que en un SO de proposito general, el SO conoce muy poco sobre el tamaño de los procesos. Sin esto SJF y STCF no funcionan bien.

## Capitulo 8: Cola MultiNivel con Retroalimentacion (MLFQ)

Politica: **MLFQ** (***Multi-Level Feedback Queue***). Busca optimizar tanto el tiempo de entrega como el tiempo de respuesta, o sea, tener a la vez buena *perfomance* y ser interactivo, brindando una respuesta adaptativa.

### Reglas Basicas

El MLFQ tiene un numero diferentes de **Colas** (***Queue***), cada una asigana a un **Nivel de Prioridad** (***Priority Level***) diferente. En un momento dado, un proceso que esta *ready* esta en una sola cola. MLFQ usa prioridades para decidir que proceso debe ejecutarse en cierto momento: Un proceso con una mayor prioridad (o sea, en una cola mas alta) es elegido para ejecutarse.
<br>Por supuesto, muchos procesos estaran en la misma cola, y por lo tanto, tendran la misma prioridad. En este caso, usaremos **RR** para planificar entre ellos.
* **Regla 1**: Si Prioridad(A) > Prioridad(B) => Se ejecuta A (B no).
* **Regla 2**: Si Prioridad(A) = Prioridad(B) => Se ejecuta A y B en RR.

La clave de MLFQ radica en como el planificador establece las prioridades. En vez de dar una prioridad fija a cada proceso, MLFQ varia la prioridad de cada proceso basado en el **Comportamiento Observado**.
<br>Por ejemplo: Un proceso cede el control de la CPU rapidamente mientras que espera por una accion del teclado, MLFQ mantendra su prioridad alta. Si en cambio, un proceso usa la CPU intensamente por largos periodos de tiempo, MLFQ reducira su prioridad. De esta forma, MLFQ trata de **Aprender** sobre los proceso a medida que se ejecutan.

### Intento 1: Como Cambiar Prioridades

Tenemos que decidir como MLFQ cambiara los niveles de prioridades de un proceso (y por lo tanto en que cola estara) a traves del tiempo de vida del trabajo. Para esto debemos pensar en la carga de trabajo: Una mezcla de procesos de corta ejecucion (pueden ceder la CPU frecuentemente), y algunos procesos de larga duracion que necesitan mucho tiempo de CPU pero que el tiempo de respuesta no es importante.
* **Regla 3**: Cuando un trabajo entra en el sistema, es ubicado en la cola de mas alto nivel de prioridad.
* **Regla 4.a**: Si un proceso usa una porcion de tiempo completa durante su ejecucion, se reduce su prioridad.
* **Regla 4.b**: Si un proceso entrega la CPU antes de que la porcion de tiempo se acabe, se mantiene en el mismo nivel.

### Ejemplo 1: Un Solo Proceso de Larga Duracion

Veamos que sucede cuando se ha estado ejecutando un proceso largo en el sistema.

![](../Teorico-practico/imagenes/MLFQEjemplo1.png)

El proceso entra en la cola de maxima prioridad (Q2). Despues de 10 milisegundos, el planificador reduce su prioridad en uno, y por lo tanto el proceso pasa a la cola Q1. Luego de ejecutarse en Q1 por un tiempo, el trabajo es bajado a la cola de mas baja prioridad en el sistema (Q0),

### Ejemplo 2: Llega un Proceso Corto

En este ejemplo hay dos procesos: `A`, el cual es un proceso de larga duracion de uso intencivo de la CPU, y `B`, el cual es un proceso de corta duracion e interactivo. Asumamos que `A` se ha estado ejecutando un tiempo, y entonces llega `B`.

![](../Teorico-practico/imagenes/MLFQEjemplo2.png)

`A` (mostrado en negro) se esta ejecutando en la cola de prioridad mas baja; cuando `B` (mostrado en gris) kkega en el tiempo $t = 100$, y por lo tanto insertado en la cola de prioridad mas alta; como su tiempo de ejecucion es corto (solo de 20 milisegundos), `B` termina su ejecucion antes de llegar a la cola de mas baja prioridad; entonces `A` retoma su ejecucion (en baja prioridad).

En este ejemplo se muestra uno de los principales **Objetivos** del algoritmo: Dado que no se sabe i un proceso sera corto o largo, primero se asume que es uno corto, por eso lo ubica en la cola de prioridad mas alta. Si realmente es un proceso corto, entonces se ejecutara rapidamente y terminara; y si no lo es, lentamente ira bajando de cola y mostrara ser un proceso largo. De esta manera MLFQ se aproxima a SJF.

### Ejemplo 3: ¿Que pasa con los I/O?

Como dice la regla 4.b, si un proceso entrega la CPU antes de usar su porcion de tiempo, entonces se mantendra el mismo nivel de prioridad. La intencion de esta regla es simple: Si un proceso interactivo, esta haciendo, por ejemplo, muchas I/O (digamos que esta esperando una entrada del teclado o mouse), cedera el control de la CPU antes de que se complete su porcion de tiempo; en tal caso, no queremos penalizar al proceso y por lo que simplemente se mantiene al mismo nivel.

![](../Teorico-practico/imagenes/MLFQEjemplo3.png)

Vemos que un proceso interactivo `B` (mostrado en gris) necesita la CPU solo 1 milisegundo antes de hacer una I/O, compitiendo con un proceso de larga ejecucion `A` (mostrado en negro). El enfoque MLFQ mantiene a `B` en la maxima prioridad. Dado que `B` mantiene liberada la CPU; si `B` es un proceso interactivo, MLFQ logra aun mas su objetivo de ejecutar proceso interactivos rapidamente.

### Problema

Problema de **Inanicion** (***Starvation***): Si hay demasiados procesos interactivos en el sistema, combinados consumirian todo el tiempo de CPU, y por lo tanto los proceso de larga duracion nunca recibirian nada del tiempo de la CPU.

### Intento 2: Impulso de Prioridad

Intentemos cambiar las reglas y veamos si podemos evitar el problema de Inanicion. La idea es elevar la prioridad de todos los procesos periodicamente. Vamos a subirlos a todos a la cola de maxima prioridad, por lo tanto hay una nueva regla:
* **Regla 5**: Despues de un periodi de tiempo `S`, mover todos los proceso del sistema a la cola de maxima prioridad.

Esta regla soluciona dos problemas a la vez:
1. Garantizamos que los procesos tengan algo de tiempo de CPU: Ubicandolo en la cola mas alta, un proceso compartira la CPU con otro proceso de maxima prioridad en forma de RR, por lo tanto, eventualemnte recivira servicio.
2. Si un proceso consumidor de CPU se vuelve interactivo, el planificador lo tratara apropiadamente una vez que se reciba el impulso de prioridad.

Ejemplo cuando un proceso de larga duracion compite por la CPU con un proceso interactivo de corta duracion.

![](../Teorico-practico/imagenes/MLFQIntento2.png)

A la izquierda no hay impulso de prioridad, por lo que el proceso largo no consume CPU una vez que llegan los dos procesos cortos.
<br>A la derecha si hay impulso de prioridad cada 50 milisegundos, por lo tanto al menos nos aseguramos que el proceso largo consumira algo de CPU, siendo impulsado a la cola de mas alta prioridad cada 50 milisegundos y por lo tanto siendo ejecutado periodicamente.

### Intento 3: Mejor Conteo

Problema a resolver: ¿Como prevenimos que los procesos jueguen con el planificador? Los culpables son las reglas 4.a y 4.b, el cual le permite a un proceso mantener su prioridad cediendo la CPU antes de que se agote su porcion de tiempo.
<br>Solucion: Hacer un mejor conteo del tiempo de CPU en cada nivel del MLFQ. En vez de olvidar cuanto de una porcion de tiempo uso un proceso en un nivel dado, el planificador le hara un seguimiento, una vez que un proceso haya hecho su parte, sera degradado de cola. Entonces si usa su porcion de tiempo de una sola vez o en varias partes, no importara. Por lo que vamos a reescribir la regla 4.a y 4.b en una sola:
* **Regla 4**: Una vez que un proceso haya usado su tiempo asignado en un nivel (independientemente de cuantas veces haya cedido la CPU), su prioridad sera reducida (o sea, se movera una cola abajo).

Ejemplo:

![](../Teorico-practico/imagenes/MLFQIntento3.png)

A la izquierda se muestra que sucede cuando una carga de trabajo intenta jugar con el planificador con las reglas 4.a y 4.b.
<br>A la derecha se muestra que sucede con la nueva regla 4. Un proceso puede lanzar una I/O justo antes de que finalice su porcion de tiempo y por lo tanto dominara la CPU. En cambio, con proteccion, sin importar el comportamiento del proceso sobre las I/O, lentamente se movera hascia abajo en las colas, y no puede ganar injustamente tiempo de CPU.

### MFLQ Final

* **Regla 1**: Si Prioridad(A) > Prioridad(B), se ejecuta A (B no).
* **Regla 2**: Si Prioridad(A) = Prioridad(B), se ejecutan A y B en RR.
* **Regla 3**: Cuando un trabajo ingresa al sistema, se coloca en la cola de prioridad más alta.
* **Regla 4**: Una vez que un trabajo utilice su tiempo asignado en un nivel dado (independientemente de cuantas veces haya renunciado a la CPU), su prioridad se reduce.
* **Regla 5**: Despues de un periodo de tiempo determinado (S), todos los trabajos del sistema se mueven a la cola de más alta prioridad.

# Virtualizacion de la Memoria

## Capitulo 13: El Espacio de Direcciones

### Tiempo Compartido (*Time Sharing*) y Multiprogramacion

Antes de la memoria virtual, los sistemas implementaban el tiempo compartido dandole a cada proceso acceso a toda la memoria fisica por un periodo corto de tiempo. Cuando el scheduler cambiaba de proceso, el SO debia guardar completamente el contenido de la memoria del proceso y cargar la del siguiete.
<br>Este enfoque tenia un problema: Era muy lento, particularmente a medida que crece la memoria. Mientras que guardar y recuperar estados de registros (PC, registros de proposito genera, etc) es relativamente rapido, guardar el contenido completo de la memoria al disco es muuy lento. Por lo tanto, lo que hicieron fue dejar el proceso en la memoria mientras cambia entre ellos, permitiendole al SO implementar el tiempo compartido mas eficientemente.

![](../Teorico-practico/imagenes/TiempoCompartido.png)

En el diagrama se observan 3 procesos (A, B y C) el cual cada uno de ellos tiene una parte chica, 512 KiloByte, de la memoria fisica reservada para ellos. Suponiendo que tenemos un solo CPU, el SO elige ejecutar uno de esos procesos (digamos que es el A) mientras que los otros (B y C) estan en la cola de procesos listos, esperando a que los ejecuten.

### Espacio de Direcciones (*Adress Space*)

El **Espacio de Direcciones** (***Adress Space***) es la vista del programa en ejecucion de la memoria en el sistema.
<br>El espacio de direcciones de un proceso contiene todo el estado de la memoria del programa en ejecucion. Por ejemplo, el codigo de un programa (las instrucciones) tienen que vivir en alguna parte de la memoria, y por lo tanto ellos estan en el espacio de direcciones.
<br>El programa, mientras se esta ejecutando, usa el ***Stack*** para mantener seguimiento de en donde se encuentra en la cadena de llamadas a funciones, asi como tambien para asignar variable locales, pasar parametros y devolver valores. El ***Heap*** es usado para asignaciones dinamicas, manejo de memoria del usuario, como las que deberia recibir de una llamada `malloc()` en C. Hay mas cosas, pero por ahora usamos solo tres componentes: *Code*, *Stack* y *Heap*.

![](../Teorico-practico/imagenes/EspacioDirecciones.png)

En el ejemplo tenemos un espacio de direcciones de 16 KiloByte. El codigo del programa vive en la parte superior del espacio de direcciones (comenzando desde el 0 en este ejemplo, y es empaquetado en la direccion 1 KiloByre del espacio de direcciones). El codigo es estatido (por lo que es facil de ubicar en la memoria), entonces podemos ubicarlo en la parte superior del espacio de direcciones y saber que no necesitamos mas memoria mientras se ejecuta el programa.
<br>Luego, tenemos dos regions que pueden **Crecer** (y **Encogerse**) mientras el programa se ejecuta. Estas estan en el *Heap* (arriba) y en el *Stack* (abajo). Las ubicamos de esta forma porque cada cual debe ser capaz de agrandarse, y poniendolas en los extremos opuestos, del espacio de memoria, les permitimos el crecimiento: Solo tiene que expandirse en direcciones opuestas. Por lo tanto el *heap* empieza justo despues del codigo (en el 1KB) y crece hacia abajo (por ejemplo, crecera cuando un usuario pide memoria via `malloc()`); el *stack* comienza en el 16KB y crece hacia arriba (por ejemplo, crecera cuando el usuario hace una llamada a un procedimiento).

Cuando decribimos el espacio de direccionamiento, lo que estamos describiendo es la **Abstraccion** que el SO proporciona a los programas en ejecucion. Realmente el programa no esta en memoria en las direcciones fisicas de 0 a 16KB; mas bien es cargada en alguna direccion fisica arbitraria. Volviendo a los procesos A, B y C del ejemplo anterior, podemos ver como cada proceso es cargado en la memoria en diferentes direcciones.
<br>Cuando el SO hace esto, decimos que el SO esta **Virtualizando la Memoria**, porque el programa en ejecucion cree que esta cargado en la memoria en una direccion particular (supongamos que es 0) y que tiene un espacio de direcciones potencialmente grande (supongamos que es de 32 o 64 bits); la realidad es direferente.
<br>Por ejemplo, cuando un proceso A intenta realizar una carga en la direccion 0 (la cual llamaremos **Direccion Virtual**), de alguna forma el SO, con la ayuda de soporte del hardware, tendra que asegurar de que la carga no vaya realmente a la direccion fisica 0, si no mas bien a la direccion fisica 320 KB (donde A esta cargado en memoria).

### Objetivos

El trabajo del SO es virtualizar la memoria. Para hacerlo, debe cumplir 3 objetivos:
1. Transparencia: La implementacion de la VM (memoria virtual) deve ser invisible para el programa, el cual debe creer que tiene su propia memoria fisica. El SO junto al hardware crean esta ilusion.
2. Eficiencia: La virtualizacion debe ser lo mas eficiente posible en terminos de tiempo y espacio. Para esto el SO utiliza distintas caracteristicas del hardware, como la TLB.
3. Proteccion: El SO debe proteger los procesos unos de otros, asi como proteger al SO de los procesos. Para eso aisla a la memoria de los proceso (solo pueden acceder a su espacio de direcciones) para que no puedan interferir entre si, permitiendo por ejemplo, que uno falle sin que afecte al resto.

## Capitulo 14: API de la Memoria

