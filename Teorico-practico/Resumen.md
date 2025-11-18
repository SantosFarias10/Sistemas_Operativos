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
- [Capitulo 14: API de la Memoria](#capitulo-14-api-de-la-memoria)
- [Capitulo 15: Traduccion de Direcciones](#capitulo-15-traduccion-de-direcciones)
- [Capitulo 16: Segmentacion](#capitulo-16-segmentacion)
- [Capitulo 17: Gestion de Espacio Libre](#capitulo-17-gestion-de-espacio-libre)
- [Capitulo 18: Introduccion a la Paginacion](#capitulo-18-introduccion-a-la-paginacion)
- [Capitulo 19: Traducciones Mas Rapidas: TLB](#capitulo-19-traducciones-mas-rapidas-tlb)

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

Crea un nuevo proceso. El proceso creado es una copia casi identica del proceso donde fue llamado `fork()` (***Parent***, **Padre**); el proceso creado (***Child***, **Hijo**) tiene una copia del ***Address Space*** (Espacio de Direcciones, se puede pensar que es el "Mapa de Memoria" privado de un proceso, Es el conjunto de todas las direcciones de memoria que el proceso *cree* que tiene disponibles) pero con su propia memoria privada, sus propios **Registros** pero con el mismo contenido, el mismo **PC**, etc.
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

### Espacio de Direcciones (*Address Space*)

El **Espacio de Direcciones** (***Address Space***) es la vista del programa en ejecucion de la memoria en el sistema.
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
1. **Transparencia**: La implementacion de la VM (memoria virtual) deve ser invisible para el programa, el cual debe creer que tiene su propia memoria fisica. El SO junto al hardware crean esta ilusion.
2. **Eficiencia**: La virtualizacion debe ser lo mas eficiente posible en terminos de tiempo y espacio. Para esto el SO utiliza distintas caracteristicas del hardware, como la TLB.
3. **Proteccion**: El SO debe proteger los procesos unos de otros, asi como proteger al SO de los procesos. Para eso aisla a la memoria de los proceso (solo pueden acceder a su espacio de direcciones) para que no puedan interferir entre si, permitiendo por ejemplo, que uno falle sin que afecte al resto.

## Capitulo 14: API de la Memoria

### Tipos de Memoria

Corriendo un programa en C, hay dos tipos de memoria:
1. El ***Stack***: las asignaciones y reasignaciones las menja el compilador **Implicitamente**. El compilador asigna la memoria necesaria y cuando ya no es necesaria la desasigna.
2. El ***Heap***: Es usada para cosas que requieren mas permanencia, donde las asignaciones y reasignaciones las realiza el usuario **Explicitamente**.

```c
void func() {
  int *x = (int *) malloc(sizeof(int));
}
```

Un par de observaciones sobre este fragmento de codigo. Primero Notar que ambas asignaciones, *stack* y *heap*, ocurren en esta linea: primer el compilador sabe hacer espacio para un puntero a un entero cuando ve la declaracion a dicho puntero (`int *x`); cuando el programa llama a `malloc()`, pide memoria para un entero en el *heap*; la rutina devuelve la direccion de dicho entero (ya sea exitoso, o `NULL` en caso de fallar), el cual es guardado en el *stack* para usar por el programador.

### `malloc()`

Consulta por espacio en el *heap* pasando el tamaño, y en cualquier csao te devuelve un puntero, al nuevo espacio asignado, o si falla devuelve `NULL`.
<br>El unico parametro que toma es de tipo `size_t`, el cual solamente describe cuantos bytes necesitas. En general no se tipea un numero, en cambio, se usan muchas rutinas y macros para hacerlo. Por ejemplo, para asignar espacio para un valor flotante de doble precision (`double`), simplemente se hace asi:

```c
double *d = (double *) malloc(sizeof(double));
```

Se usa el operador `sizeof()` para solicitar la cantidad correcta de espacio.
<br>Tambien se le puede pasar el nombre de una variable (y no solo el tipo) a `sizeof()`, pero en algunos casos se puede no obtener los resultados esperados. Por ejemplo:

```c
int *x = malloc(10 * sizeof(int));
printf("%d\n", sizeof(x));
```

En la primera linea, declaramos espacio para un array de 10 `int`. Sin embargo, cuando usamos `sizeof()` en la siguiente linea, devuelve un numero chico, 4 para maquinade de 32 bits y 8 para maquinas de 64 bits. La razon es que `sizeof()` cree que le estamos preguntando por el tamaño de un puntero a un entero, no sobre cuanta memoria tenemos asignada dinamicamente. Pero, a veces `sizeof()` funciona como se espera

```c
int x[10];
printf("%d\n", sizeof(x));
```

En este caso, hay suficiente informacion estatica para que el compilador sepa que fueron asignados 40 bytes.

Cuando declaramos espacio para un `string`, usamos el siguiente formato: `malloc(strlen(s) + 1)`, el cual obtiene el largo del `string` usando la funcion `strlen()`, y se le suma uno para hacer espacio para el caracter de fin del `string`.

Notar que `malloc()` devuelve un puntero a `void`. Esta es la forma que tiene C para pasar una direccion al programador para dejarle decidir que hacer con ella.

Tambien existe `calloc()` que inicializa la memoria asignada en cero. Y `realloc()` copia una region de memoria y le asigna un espacio de tamaño diferente.

### `free()`

Para liberar memoria del *heap* simplemente llamamos a `free()`:

```c
int *x = malloc(10 * sizeof(int));
...
free(x)
```

Toma como argumento un puntero retornado por `malloc()`. Notar que el tamaño de la region asignada no es pasada por el usuario, y debe ser rastreada por la misma libreria de asignacion de memoria.

Tanto `malloc()` y `free()` no son *system calls*; son parte de la libreria de manejo de memoria `stdlib`. Las *system calls* son `brk()` (toma como argumento la direccion del nuevo final) y `sbrk()`, que mueven el *break*; la memoria maxima a la que puede acceder el *heap*, y `mmap()`, que permite mapear un archivo en memoria.

### Errores Comunes

Algunos lenguajes tienen un manejo de memoria automatico (***garbage collector***), lo que hace que no sea necesario liberar memoria explicitamente. En aquellos lenguajes que no tienen *garbage collector* (como C), se puede ocacionar algunos errores comunes:
* Olvidarse de asignar memoria: ***Segmentation Fault***. Muchas rutinas esperan ya tener memoria asignada al ser llamadas (por ejemplo `strcpy()`).
* No asignar memoria suficiente: ***Buffer Overflow***. Normalmente se asigna la memoria justa, algunas veces `malloc()` deja un poco de margen para evitar estos errores. Cuando no es suficiente, un *overflow* de bytes puede ocurrir.
* Olvidarse de inicializar la memoria asignada: ***Uninitialized Read***. Si se llama a `malloc()` pero no se le asigna valores a la memoria asignada, el programa eventualmente puede acceder a ella y leer del *heap* informacion de valor desconocido, causando comportamientos no previstos.
* Olvidarse de liberar memoria: ***Memory Leak***. Cuando ya no se usa, la memoria debe ser liberada. De no hacerlo, puede llevar a acabar la memoria en aplicaciones *long-running*.
* Liberar memoria que todavia se puede necesitar: ***Dandling Pointer***. Si un programa libera memoria antes de usarla se queda un puntero colgante, llegando a crashear o sobreescribir memoria valida.
* Liberar memoria repetidamente: ***Double Free***. Los programas pueden intentar liberar memoria mas de una vez, lo cual genera comportamientos indefinidos o crasheos.
* Llamar a `free()` incorrectamente: ***Free Incorrectly***. `free()` solo espera que se le pase como argumento un puntero devuelto por `malloc()`. Cn cualquier otro valor o argumento el `free()` es invalido.

## Capitulo 15: Traduccion de Direcciones

En el desarrollo de la virtualizacion del CPU nos centramos en el mecanismo general LDE, Ejecucion Directa Limitada (*Limited Direct Execution*), cuya idea es dejar el proceso correr en el hardware la mayo parte del tiempo, y que en ciertos puntos clave el SO se involucre y tome decisiones que le permitan asegurarse, con ayuda del hardware, mantener el control mientras trata de mantenerse fuera del camino del proceso.

En la **Virtualizacion de la Memoria** se busca algo similar; obtener **Control** y **Eficiencia** mientras se provee la virtualizacion. La eficiencia es lo que dicta que se use el apoyo del hardware. Controlar implica que el SO asegure que ninguna aplicacion tenga permitido acceder a otra memoria salvo la suya, y asi proteger aplicaciones unas de otras y al SO de las aplicaciones (lo que tambien requiere ayuda del hardware).

Algo que necesitaremos del sistema de memoria virtual (VM) es **Flexibilidad**; que los procesos puedan usar su *address space* como quieran, haciendo asu el sistema mas sencillo.

### Virtualizar Memoria Eficiente y Flexiblemente

La idea es realizar una ***hardware-based address translation*** (**Traduccion de Direcciones en Base a Hardware**). Con la **Traduccion de Direcciones**, el hardware transforma cada acceso a memoria (*fetch*, *load*, *store*), cambiando la **Direccion Virtual** provista por la instruccion a una **Direccion Fisica** donde esta guardada la informacion deseada. Por lo tanto, en cada referencia a memoria, un traductor de direcciones es ejecutado por el hardware para redirigir referencia de memoria de la aplicacion a la ubicacion real en memoria.
<br> El hardware no virtualiza la memoria, solo provee un mecanismo de bajo nivel para lograrlo eficientemente. Es el SO quien se involucra y maneja la memoria, sabiendo que localizaciones estan libres y cuales en uso, y manteniendo asi control sobre como la memoria es usada.

### Suposiciones

Por ahora asumiremos que el *address space* del usuario debe ser ubicado contiguamente en la memoria fisica. Tambien asumiremos que el tamaño del *address space* no es muy grande; especificamente, que es mas chico que el tamaño de la memoria fisica. Por ultimo, asimiremos que cada *address space* tiene el mismo tamaño.

#### Ejemplo

Imaginemos un proceso cuyo *address space* es:

![](../Teorico-practico/imagenes/SuposicionEjemplo.png)

Lo que vamos a examinar es una secuencia de codigo que carga un valor desde la memoria, lo incrementa en 3, y lo guarda de nuevo en la memoria. Se puede imaginar en codigo en C como:

```c
void func() {
  int x = 3000;
  x = x + 3;  // Parte del codigo que nos interesa
}
```

El compilar traduce esta linea de codigo a assembly, la cual se ve algo asi:

```assembly
movl  0x0     (%ebx),  %eax    ; carga 0+ebx en eax
addl  $0x03,  %eax             ; suma 3 al registro eax
movl  %eax,   0x0      (%ebx)  ; guarda eax de vuelta a memoria
```

Se asume que la direccion de `x` ha sido puesta en el registro `ebx`, entonces carga el valor de esa direccion en el registro de proposito general `eax` usando la instruccion `movl`. La siguiente instruccion `add 3` a `eax`, y la instruccion final guarda el valor de `eax` de nuevo en la memoria en la misma ubicacion.
<br>Volviendo a la imagen, se observa como el codigo y los datos son puestos en el *address space*; la secuencia de codigo de tres instrucciones esta ubicada en la direccion 128, y el valor de la variable `x` en la direccion 15KB. El valor inicial de `x` es 3000.
<br>Cuando se ejecutan las instrucciones, desde la perspectiva del proceso, se ejecutan los siguientes accesos a memoria:
* *Fetch*: Se busca la instruccion en la direccion 128.
* *Execute*: Se ejecuta la instruccion (cargar la direccion 15KB).
* *Fetch*: Se busca la instruccion en la direccion 132.
* *Execute*: Se ejecuta la instruccion (sin referencia a memoria).
* *Fetch*: Se busca la instruccion en la direccion 135.
* *Execute*: Se ejecuta la instruccion (guardar en la direccion 15KB).

Desde el punto de vista del programa, su *address space* comienza en la direccion 0 y crece hasta el maximo de 16KB; todas las referencias de memoria que genera deben estar en este rango. Pero, al virtualizar la memoria, el SO quiere ubicar el proceso en cualquier parte de la memoria fisica, no necesariamente en la direccion 0. 

Vemos un ejemplo de como se deberia ver la memoria fisica una vez que el *address space* del proceso ha sido ubicado en memoria:

![](../Teorico-practico/imagenes/SuposicionEjemplo2.png)

Podemos ver al SO usando el primer slott de la memoria fisica para el mismo, y que ha reubicado el proceso en el slot que empieza en la direccion 32 KB de la memoria fisica. Los otros slots estan libres.

### Reubicacion Dinamica (*Hardware-Based*)

Para virtualizar la memoria, el SO debe poner a cada programa en un lugar diferente a la direccion 0. Para hacerlo de forma transparente (sin que el proceso se de cuenta), se usan las ideas de ***Base and Bounds*** (**Base y Limite**); aunque tambien se los conoce como ***Dynamic Relocation*** (**Reubicacion Dinamica**); se usan ambos terminos de forma indiscriminada xd.
<br>Especificamente, se necesitan dos registros del hardware en cada CPU: Uno es llamado registro **Base** y el tro **Limite** (***Bounds***). Este par base-limite nos permite ubicar el *address space* en cualquier lugar que querramos de la memoria fisica, y asegurarnos que el proceso solo pueda acceder a su *address space*.
<br>Como cada programa cree estar en la direccion 0, es el SO quien al cargarlos decide donde ponerlos en memoria fisica, y establece los registros base-limite con ese valor. En el ejemplo anterior, el SO decide cargar el proceso en la direccion fisica 32KB y por lo tanto configurar el registro base con ese valor.

Cualquier referencia a memoria generada por el programa sera traducida por el procesador usando:

$$
Physical\_Address = Virtual\_Address + Base
$$

Cada referencia a memoria creada por el proceso es una **Direccion Virtual**, el hardware almacena los contenidos de la base a la direccion y el resultado es la **Direccion Fisica**

Transformar una direccion virtual en una direccion fisica es la tecnica a la que nos referimos con **Traduccion de Direcciones**. Dado que la reubicacion de la direccion sucede en tiempo de ejecucion, y dado que podemos mover el *addres space* incluso despues de que el proceso comienza a ejecutarse, esta tecnica es conocida como **Reubicacion Dinamica**.

El registro **Limite** esta para ayudar con la **Proteccion**. Especificamente, el proceso primero verifica que es legal; en el ejemplo anterior, el registro limite siempre estara seteado en 16KB. Si un proceso genera una direccion virtual que es mas grande que el limite, o es negativa, la CPU lanzara una excepcion, y probablemente el proceso sera detenido. El objetivo del limite es asegurar que todas las direcciones generadas por el proceso sean legales y este dentro de los limites de ese proceso.

Notar que los registros base-limite son estructuras del hardware que esta ubicados en el chip (un par por cada CPU). A esta parte del procesador que ayuda con las traducciones de direcciones se las llama ***Memory Management Unit*** (**MMU**) (***Unidad de Administraccion de Memoria***)

### Ejemplo de Traduccion

Imaginemos un proceso con un *address space* de tamaño de 4KB, que ha sido cargado en la direccion fisica 16KB. Aca estan los resultado de algunas de las traducciones de direcciones.

| Direccion Virtual | | Direccion Fisica |
| :---: | :---: | :---: |
| 0 |&rarr;| 16KB |
| 1KB |&rarr;| 17KB |
| 3000 |&rarr;| 19384 |
| 4400 |&rarr;| *Fault (Fuera del Limite)* |

Como vemos, simplemente agregamos la direccion base a la direccion virtual (la cual puede ser vista como un *offset* dentrol del *address space*) para obtener la direccion fisica resultante.

### Soporte del Hardware

El hardware debe proveer al SO distintos mecanismos para lograr la virtualizacion de la memoria:
* La posibilidad de soportar dos modos de ejecucion (usuario y kernel).
* Proporcionar los registros base-limite.
* La capacidad de **Chequear** que la memoria a la que se intenta acceder se encuentre dentro de los limites de base-limite y, en ese caso, **Traducir** la memoria virtual/fisica.
* Otorgar las **Instrucciones Privilegiadas** para que el SO pueda **Modificar** los registros base y limite (igual que poder modificar los *exception hablders*), mientras un proceso esta en ejecucion.
* Generar excepciones cuando un programa trata de acceder a memoria ilegal o fuera de su *address space*, parando el proces y retornando el control al SO corriendo el *Exception handler*; lo mismo que ocurre si un proceso trata de ejecutar una instruccion privilegiada.

### Problemas del SO

Usando las herramientoas proporcionadas por el hardware, el SO logra la virtualizacion de la memoria. Para ello, cuenta con tres responsabilidades:
1. Administracion de la memoria (*heap*): Encontrar Espacio para el *address space* de un proceso cuando este es creado, para lo cual el SO busca el espacio en la estructura de datos (***Free List***) y la asigna. Luego, cuando un proceso termina (por si mismo o la fuerza por el SO) debe quitarlo del *scheduler*, reclamar la memoria y agregar dicho espacio a la *free list*.
2. Manejo de los registros base-limite: Debe guarda y restaurar los registros base-limite en cada *context switch*, guardar sus valores en memoria (en alguna estructura por cada proceso, como la estructura del proceso mismo (***Process Structure*** o ***Process Control Block***; **PCB**)), y al restaurarlo debe pasarle dicho valores al CPU.
3. Definicion de los ***Exception Handlres*** (**Manejo de Excepciones**) en el momento de *booteo*, para luego ser ejecutados en caso de accesos a memoria ilegal (errores ***Out of Bounds***; fuera de rango) o intentos de uso de instrucciones privilegiadas.

## Capitulo 16: Segmentacion

Hasta ahora hemos estado poniendo todo el *address space* de cada proceso en memoria. Con los registros base-limite, el SO puede facilmente reubicar los procesos a diferentes partes de la memoria. Pero, notar que los *address space* tienen un gran chunk de espacio libre justo en el medio, entre el *stack* y el *heap*.

![](../Teorico-practico/imagenes/Segmentacion.png)

Como vemos, a pesar de que el espacio entre el *stack* y el *heap* no esta siendo usado por el proceso, aun toma memoria fisica cuando reubicamos el *address space* entero en algun lugar de la memoria fisica; por lo que, el enfoque simple de usar registros base-limite es poco economico. Tambien hace dificil ejecutar un programa cuando el *address space* entero no esta en memoria; por lo tanto, base-limite no es tan flexible.

### Segmentacion: Base-Limite Generalizado

La idea de **Segmentacion** es dividir el *address space* de un proceso en **Segmentos Logicos** (*code*, *heap* y *stack*). Cada segmento va a tener su propio registro base-limite en la MMU. Esto permite colocar cada semgmento en distintas partes de la memoria fisica y no tener que reservar todo el *address space* completo. La memoria fisica se usa solo para los segmentos realmente utilizados.

Ejemplo: Asumamos que queremos ubicar el *address space* de la imagen anterior en la memoria fisica. Con un par base-limite por segmento, podemos ubicar cada segmento **Independientemente** uno de otro en la memoria fisica.

![](../Teorico-practico/imagenes/SegmentacioBaseLimiteGeneralizdo.png)

Se puede ver una memoria fisica de 64KB con tres segmentos en ella (hay 16KB reservados para el SO).
<br>Solo la memoria que es usada tiene asignado un espacio en la memoria fisica, y por lo tanto, *address spaces* mas grandes con mas cantidad de *adress space* no usadas (llamados ***Sparse Address Space*** (**Espacios de Direcciones Escasos**)) se pueden acomodar.
<br>Lo que se esper que que la estructura de hardware de nuestra MMU requiera soporte de semgmentacion: En este caso, un conjunto de tres pares de registrso base-limites.

| Segmentos | Base | Tamaño |
| :---: | :---: | :---: |
| *Code* | 32K | 2K |
| *Heap* | 34K | 3K |
| *Stack* | 28K | 2K |

Cada segmento limite mantiene el tamaño de un segmento.
<br>En la tabla se puede ver que el segmento de codigo fue ubicado en la direccion fisica 32KB y que tiene un tamaño de 2KB, y el segmento del *heap* fue puesto en la direccion 34KB con un tamaño de 3KB. El tamaño del segmento es lo mismo que el registro limite; le dice al hardware exactamente cuantos bytes son validos para ese segmento.

Ejemplo de traduccion, tomando la imagen del ejemplo anterior. Asumamos que se hizo una referencia a la direccion virtual 100 (la cual esta en el segmento del codigo). Cuando la referencia toma lugar, el hardware agrega el valor base al *offset* dentro de ese segmento para llegar a la direccion fisica deseada: 100 + 32KB (32KB = 32768), o 32868. Entonces verificara que la direccion este dentro de los limites (100, 2KB), la encontrara, y emitira una referencia a la direccion de la memoria fisica 32868.
<br>O sea, los pasos generales para traducir una direccion virtual con segmentacion:
1. Identificar el segmento al que pertenece la Direccion virtual (*code*, *heap*, *stack*).
2. Obtener el inicio virtual del segmento (`base_virtual_segmento`).
3. Calcular el *offset* dentro del segmento:

```ini
offset = direccion_virtual - base_virtual_segmento
```

4. Verificar el limite del segmento: Comprobar $offset < limite$.
   * Si no se cumple => *segmentation fault*.
5. Obtener la base fisica del segmento (`base_fisica_segmento`).
6. Calcular la direccion fisica:

```ini
Direccion_Fisica = base_fisica_segmento + offset
```

7. Usar la direccion fisica para acceder a memoria fisica.

Entonces en el ejemplo anterior tenemos que se hizo una referencia a la direccion virtual 100, la cual esta en el segmento del codigo (o sea la base virtual es 0). Sabemos que el limite (tamaño) de `code` es de 2KB (2048) y que la base fisica es de 32 KB (32 * 1024 = 32768).
<br> Entonces el *offset* es igual a la direccion virtual (100) - base virtual (0), tenemos que el offset es 100, como 100 es menor que 2KB (2048) (limite) entonces es valida. Luego para calcular la direccion fisica sumamos la base fisica (32 KB = 32768) mas el offset que calculamos (100), y nos da que la direccion fisica es 32868.

Ahora vemos en una direccion del *heap*, direccion virtual 4200. La base del *heap* es de 34KB (34KB = 34816). Dado que el *heap* comienza en la direccion virtual 4KB (4096) (base virtual), el *offset* de 4200 es realmente es la direccion virtual (4200) menos la base virtual (4096), o sea 104. Como el limite del *heap* es 3KB, entonces el *offset* es valido. Entonces tomamos ese *offset* y se lo sumamos al registro base de la direccion fisica (34KB = 34816) para obtener la direccion fisica deseada: 34920.

### ¿A que Segmento Nos Referimos?

El hardware usa registros de segmentos durante la traduccion. ¿Como sabe cual es el *offset* dentro de un segmento, y a que segmento pertenece una direccion?
<br>Un enfoque, a veces referido como un enfoque **Explicito**, es cortar el *address space* en funcion de los bits superiores de una direccion virutal. En el ejemplo anterior, tenemos tres segmentos; por lo que necesitamos dos bits para lograr nuestra tarea (dos bits ya que se puede contar hasta 4 con esa cantidad; 00, 01, 10, 11). Si usamos los primero dos bits de nuestra direccion virtual de 14 bits para seleccoinar el segmento, nuestra direccion virtual se tendria que ver algo asi:

![](../Teorico-practico/imagenes/SegmentacionPregunta.png)

Si los primeros dos bits son 00, el hardware sabra que la direccion virtual esta en el segmento de *code*, y por lo tanto usara el par base-limite del codigo para reubicar la direccion a la ubicacion fisica correcta. Si los primeros dos bits son 01, el hardware sabra que la direccion es del *heap*. Tomemos del ejemplo la direccion virtual del *heap* (4200) y la traduzcamo. La direccion virtual 4200, en binario, es:

$$
01_{segmento} |  000001101000_{offset}
$$

Los primeros dos bits (01) ke dicen al hardware a que segmento nos estamos refiriendo. Los utlimos 12 bits son el *offset* del segmento: 0000 0110 1000, o 0x068 en hexa, 0 104 en decimal. Sumando el registros base al *offset*, el hardware llega a la direccion fisica final. Notar que el *offset* facilita verificar el limite: Podemos simplemente verificar que el *offset* sea menos que el limite; si no lo es, la direccion es ilegal.

### ¿Que pasa con el *stack*?

El *stack* a sido reubicado en la direccion fisica 28KB en el diagrama anterior, pero con una diferencia: Crece hacia atras. En la memoria fisica, comienza en 28KB y crece hacia atras hasta 26KB, que corresponde a la direccion virtual 16KB a 14KB, por lo que la traduccion tiene que ser diferente.
<br>Lo primero que necesitamos es hardware. En vez de solo valores de base-limite, el hardware tambien necesita saber en que direccion crece el segmento (por ejemplo, un bit, 1 cuando el segmento crezca en direccion positiva o 0 para direccion negativa).

| Segment | Base | Size(max 4K) | Grows positive? |
| :---: | :---: | :---: | :---: |
| $$Code_{00}$$ | 32K | 2K | 1 |
| $$Heap_{01}$$ | 34K | 3K | 1 |
| $$Stack_{11}$$ | 28K | 2K | 0 |

Ahora que el hardware conoce que segmentos pueden crecer de forma negativa, ahora traduce las direcciones virtuales de forma diferente. Tomamos un ejemplo de direccion virtual del *stack* y la traducimos.
<br>Asumamos que queremos acceder a la direccion virtual 15KB, la cual debe ser mapeada en la direccion fisica 27KB. Nuestra direccion virtual, en binario es 11 1100 0000 0000 (en hexa 0x3C00). El hardware usa los primeros dos bits (11)para designar el segmento, pero entonces estamos dejando un *offset* de 3KB. Para obtener el *offset* negativo, debemos restarle a los 3KB el tamaño maximo del segmento: En el ejemplo, un segmento puede ser de 4KB, por lo que el *offset* negativo es 3KB menos 4KB, lo cual es igual a -1KB. Simplemente sumamos el *offset* negativo a la base (28KB) para llegar a la direccion fisica: 27KB. La verificacion del limite la puede calcular asegurando que el valor absoluto del *offset* negativo es menor o igual que el tamaño del segmento actual (en este caso, 2KB).

### Soporte para compartir

Para guardar memoria, a veces es util **Compartir** ciertos segmentos de memoria entre *address space*. En particular, ***Code Sharing*** es comun.
<br>Para soportar compartir, necesitamos ayuda del hardware, de forma de **Proteccion de Bits**. Un soporte basico agrega pocos bits por segmento, indicando ya sea que un segmento puede o no leer o escribir un segment. Seteando un segmento de codigo como lectura, el mismo codigo puede ser compartido a traves de multiples procesos, sin preocuparnos de dañar el aislamiento; mientras que cada proceso aun cree que esta accediendo a su propia memoria privada, el SO secretamente esta compartiendo memoria que no puede ser modificada por el proceso, y por lo tanto la ilusion se mantiene.

### Segmentacion: Grano Fino vs Grano grueso

En los ejemplos que dimos han sido centrados en sistemas de pocos segmentos (*code*, *heap*, *stack*); podemos pensar que nuestra segmentacion es de **Grano Grueso**, como corta los *address space* en granos de chunks relativamente grandes. Pero, algunos de los primeros sistemas eran mas flexibles y le permitian a los *address space* consistir de un mayor numero de segmentos mas chivos, y se conocia como segmentacionde **Grano Fino**.
<br>Soportar muchos segmentos requiere mas ayuda del hardware con una **Tabla de Segmentos**. Como las tablas de segmentos soportaban la creacion de un numero mas grande de segmentos, entonces esto les permitio al sistema usar de forma mas flexible los segmentos.

### Soporte del SO

De la segmentacion surge un nuevo numero de problemas para el SO. ¿Que debe hacer el SO en un *context switch*? Los registros de los segmentos deben ser guardados y recuperados. Claramente, cada proceso tiene su propio *address space*, y el SO debe asegurarse de iniciar estos registros correctamente antes de permitirle a un proceso ejecutarse nuevamente.
<br>Otro problema es la interaccion del SO cuando un segmento crece o se reduce. Por ejemplo, un programa puede llamar a `malloc()` para guardar un objeto. En algunos casos, el *heap* existente sera capaz de cumplir el requerimiento, y por lo tanto `malloc()` encontrara espacio libre para el objeto y retornara un puntero a el. En otro caso, el segmento *heap* necesitara crecer. En este caso, la libreria de asignacion de memoria hara una *system call* para agrandar el *heap*. El SO entonces proporsionara mas espacio, actualizando el registro del tamaño del segmento por el nuevo tamaño, e informando a la libreria; entonces la libreria puede asignar espacio para el nuevo objeto y retornar exitosamente. Notar que el SO podria rechazar la peticion si no hay mas memoria fisica disponible o si decide que el proceso que la pidio ya tiene demasiada.
<br>El ultimo problema es la gestion del espacio libre en la memoria fisica. Cuando se crea un nuevo *address space*, el SO tiene que ser capaz de encontrar espacio en la memoria fisica para sus segmentos. Antes, asumimos que cada *address space* tenia el mismo tamaño, por lo que la memoria fisica se podia pensar como un puñado de slots donde se podian poner los procesos. Ahora, tenemos un numero de segmentos por proceso, y cada segmento tiene un tamaño diferente.
<br>El problema general es que la memoria fisica se llena de huevos de espacio libre, haciendo dificil la asignacion de memoria a nuevos segmentos, o que crezcan los que ya existen. Este problema es llamado **Fragmentacion Externa**.

![](../Teorico-practico/imagenes/FragmentacionExterna.png)

Supongamos que llega un proceso y desea guardar un segmento de 20KB. En la imagen, hay 24KB libres, pero no es un segmento contiguo. Por lo tanto, el SO no puede satisfacer la peticion de 20KB.
<br>Una solucion podria ser **Compactar** la memoria fisica reorganizando los segmentos existentes. Por ejemplo, el SO podria frenar cualquier proceso que se este ejecutando, copiar sus datos en una region contigua de la memoria, cambiar sus valores de los registros del segmento al punto de la nueva ubicacion, y por lo tanto tener una extencion mas grande de memoria libre con la cual trabajar. Pero, compactar sale caro, copiar segmentos usa mucha memoria y una gran cantidad de tiempo de procesador.

![](../Teorico-practico/imagenes/MemoriaCompactada.png)
* Ejemplo de memoria compactada.

## Capitulo 17: Gestion de Espacio Libre

El manejo del espacio libre resulta sencillo cuando el espacio esta dividido en unidades de espacio constante; si se solicita espacio solo entregar la primera entrada libre (**Paginacion**). Pero se vuelve dificil al tratar con bloques de espacio libre de diferentes tamaños.

![](../Teorico-practico/imagenes/GestionDeEspacioLibre.png)

En la imagen se ve un ejemplo de este problema. En este caso, el espacio total disponible es de 20 bytes; desafortunadamente, esta fragmentado en dos *chunks* de 10 bytes cada uno. Como resultado, una peticion por 15 bytes fallara a pesar de que hay 20 bytes libres.

### Suposiciones

Asumiremos una interfaz como la que proporsiona `malloc()` y `free()`. Especificamente `void * malloc(size_t size)` tomoa un solo parametro (`size`), el cual es el numero de bytes pedidos por la aplicacion; y le llega un puntero a la region de ese tamaño. La rutina complementaria `void free(void *ptr)` toma un puntero y libera el chunk correspondiente.
<br>La ***Free List*** es una estructura que contiene referencias a todos los chunks libres del espacio de la region de memoria administrada.
<br>Tambien asimos que nos vamos a centrar en la **Fragmentacion Externa**. Los asignadores tambien tienen problemas de **Fragmentacion Interna**; si un asignador recibe chunks de memoria mas grandes de los que pidio, tal espacio es considerado **Fragmentacion Interna**.
<br>Tambien supondremos que una vez que la memoria es entregada a un cliente no puede ser reubicada en otra ubicacion de la memoria. Una vez que se llama a `mallo()` con memoria dentro del *heap*, esta no puede ser tocada por la libreria hasta que se use `free()`. Por lo tanto, no compacteremos el espacio libre si es posible, lo cual debera ser util para combatir la fragmentacion. La compactacion deberia, sin embargo, ser usada en el SO para tratar con la fragmentacion cuando se implementa **Segmentacion**.
<br>Por ultimo, se supone que el asignado administra una region contigua de bytes.

### Mecanismos de Bajo nivel

#### Division y Fusion
Una *free list* contiene un conjunto de elementos que describen el espacio libre que todavia queda en el *heap*. Por lo tanto, asumamos el siguiente *heap* de 30 bytes:

![](../Teorico-practico/imagenes/GestionDeEspacioLibre.png)

La *free list* para este *heap* deberia tener dos elementos. Una entrada describe el primer segmento de 10 bytes (bytes 0-9), y otra entrada describe el otro segmento libre (bytes 20-29).

![](../Teorico-practico/imagenes/DivisionYFusion.png)

Una petision por cualquier cosa mas grande de 10 bytes va a fallar y rotornara `NULL`; no hay un unico chunk contiguo de memoria disponible de este tamaño. Una peticion para exactamente 10 bytes se podria satisfacer por cualquier chunk libre.
<br>Supongamos que tenemos una peticion de un solobytes de memoria. En este caso, el asignador hara una accion conocida como **Division**: Encontrara un chunk libre de memoria que pueda satisfacer la peticion y lo dividira en dos. El primer chunk lo retornara al que hizo la peticion; y el segundo permanecera en la lista. Por lo tanto, en el ejemplo, si se hace una peticion por 1 byte, el asignador decide usar el segundo de los elementos de la lista para satisfacer la peticion, la llamada a `malloc()` retornara 20 (la direccion de la regoin de 1 byte) y la lista quedara algo asi:

![](../Teorico-practico/imagenes/DespuesDeLaDivision.png)

Se puede ver que la lista se mantuvo intacta; el unico cambio es que la region libre ahora comienza en 21, y el largo de esa region ahora es 9. Por lo tanto, **la division es comunmente usada en los asignadores cuando una peticion es menor que el tamaño de algun chunk libre** en particular.
<br> Otro mecanismo usado en muchos asigadores es conocido como **Fusion** del espacio libre. Tomemos el ejemplo del principio (10bytes libres, 10 bytes usados y otrso 10 bytes libres).
<br>Dado este *heap*, ¿Que pasa cuando una aplicacion llama a `free(10)`? Si lo sumamos al espacio libre de nuevo a nuestra lista, terminaremos con una lista como esta:

![](../Teorico-practico/imagenes/DespuesDelFree.png)

El problema: A pesar de que el *heap* entero esta libre, parece estar dividido en tres chunks de 10 bytes cada uno. Por lo tanto, si el usuario hace una peticion de 20 bytes, no encontrara un chink libre de esas caracteristicas y fallara.
<br>Lo que hacen los asignadores para evitar este problema es **Fusionar** el espacio libre cuando un chunk de memoria es liberado. Cuando retorna un chunk liberado en memoria, mira que esta retornando y a los chunks libres cercanos; si el nuevo espacio libre esta al lado de un (o dos) chunks libres, los ordena en un solo chunk mas grande. Por lo tanto, con la Fusion, nuestra lista seria:

![](../Teorico-practico/imagenes/DespuesDeLaFusion.png)

### Rastreando el Tamaño de las Regiones Asignadas

Notar que la interfaz para *free(void *ptr)* no toma por parametro el tamaño; por lo que es asumido por el puntero dado, la libreria `malloc()` puede determinar rapidamente la dimension de la region de memoria a libera y puede incorporar de vuelta en la *free list*.
<br>Muchos asignadores guardan un poco de informacion extra en el *header* del bloque el cual mantiene en la memoria, usualmente justo antes de devolver el chunk de memoria. Veamos un ejemplo:

![](../Teorico-practico/imagenes/Rastreando.png)

En este ejemplo estamos examinando un bloque asignado de 20 bytes, apuntado por `ptr`; imaginemos que el usuario llamo a `malloc()` y guardo el resultado en `ptr`, o sea, `ptr = malloc(20)`.
<br>El *header* como minimo contiene el tamaño de la region asignada (en este caso 20); y tambien debe contener punteros adicionales para acelerar la desasignacion, un numero magico para proporcionar verificacion de integridad, y otra informacion. Asumamos un *header* simple el cual contiene el tamaño de la region y un numero magico:

```c
typedef struct{
  int size;
  int magic;
} header_t;
```

El ejemplo podria verse algo asi:

![](../Teorico-practico/imagenes/NumeroMagico.png)

Cuando el usuario llama a `free(ptr)`, la libreria usa un puntero aritmetico simple para descubrir donde comienza el *header*:

```c
void free(ptr){
  header_t *hptr = (header_t *) ptr-1;
  ...
  }
```

Despues de obtener dicho puntero al *header*, la libreria puede determinar facilmente si el numero magico coincide con el valor esperado (`assert(htpr->magic) == 1234567`) y calcular el tamaño total de la region recien liberada por medio de la matematica (o sea, sumando el tamaño del *header* al tamaño de la region). Notar que el tamaño de la region libre es el tamaño del *header* mas el tamaño del espacio asignado por el usuario. Por lo tanto, cuando un usuario pide N bytes de memoria, la libreria no busca un chunk libre de tamaño N si no, busca un chunk libre de tamaño N mas el tamaño del *header*.

Resumen rapido por si no quedo claro el tema del ***Header*** (**Encabezado**). Cuando el asignador (`malloc()`) te da memoria, no te da el bloque solo. En realidad, te da:

```
[ HEADER ]
[ BLOQUE QUE EL PROGRAMA USA ]
```

El *Header* es una estructura (unos poco bytes) que esta justo antes del bloque que recibimos. Sirve para guardar informacion que el sistema necesita. Por ejemplo:
* El tamaño del bloque asinado.
* Si el bloque esta libre o ocupado.
* Un puntero al siguiente bloque.
* etc.

Se guarda el tamaño del bloque para cuando se llama a `free(ptr)` el sistema no sabe cuanto mide el bloque que le estas devolviendo, nosotros solo le pasamos un puntero. Entonces, se necesitar mirar el *header* qye guardo antes para saber.

#### Insertanto una *Free List*

En una lista mas normal, cuando se asigna un nuevo nodo, solo llamamos a `malloc()` cuando necesesitamos espacio para el nodo. Pero, en la libreria de asignacion de memoria, no se puede hacer esto. En cambio, se necesita construir la lista dentro del mismo espacio libre.
<br>Asumamos que tenemos un chunk de memoria de 4096 bytes para administrar (o sea, el *heap* es de 4KB). Para administrar esta *free list*, primero tenes que inicializar dicha lista; inicialmente, la lista deberia hacer una entrada, de tamaño 4096 (menos el tamaño del *header*). La descripcion de un nodo de la lista se veria algo asi:

```c
typedef struct __node_t {
  int size;
  struct ___note_t * next;
} node_t;
```

Estamos asumiendo que el *heap* es construido de algun espacio libre adquirido via una llamada a la *system call* `mmap()`; esta no es la unica forma para construir dicho *heap* pero nos sirve especificamente para este ejemplo. El codigo:

```c
// mmap() returns a pointer to a chunk of free space
node_t *head = mmap(NULL, 4096, PORT_READ|PORT_WRITE,
                    MAP_ANON|MAP_PRIVATE, -1, 0);
head->size = 4096 - sizeof(t);
head->next = NULL;
```

Despues de ejecutar este codigo, el *heap* contiene una *free list* con un solo nodo, de tamaño 4088. El puntero *head* contiene el inicio de la direccion de este rango; vamos a asumir que es 16KB. Visualmente, el *headp* se veria asi:

![](../Teorico-practico/imagenes/FreeList.png)

Ahora supongamos que un chunk de memoria es pedido, digamos de tamaño de 100 bytes. Para cumplir con la peticion, la libreria encontrara un chunk de largo suficiente; dado que hay un solo chunk libre (de tamaño 4088), elegira este chunk. Entonces, el chunk sera dividido en dos: Un chunk suficientemente grande para servir a la peticion (mas el *header*), y el resto como chunk libre. Asumamos un *header* de 8 bytes, el espacio en el *heap* ahora se veria asi:

![](../Teorico-practico/imagenes/FreeListDivididaEnDos.png)

Por lo tanto, por la peticion de 100 bytesm la libreria asignara 108 bytes del existente chunk libre, retornara un puntero a el, escondera la informacion del *header* inmediatamente antes de la asignacion de espacio para el futuro uso de `free()`, y reducira el unico nodo libre en la lista a 3980 bytes (4088-108).
<br>Ahora veamos al *heap* cuando tiene asignada tres regiones, cada una de 100 bytes (108 incluyendo el *header*):

![](../Teorico-practico/imagenes/TresSecciones.png)

Como vemos, los primeros 324 bytes del *heap* ahora estan asignados, y se puede ver tres *headers* en ese espacio. La *free list* sigue siendo poco interesante: Tiene un solo nodo (siendo apuntado por *head*), pero ahora solo tiene 3764 bytes de tamaño despues de las tres divisiones.
<br> Si el programa devuelve memoria usando `free()`, entonces la aplicacion devuelve medio chunk de memoria asignada, llamando a `free(16500)` (se llega al valor 16500 sumando el inicio de la region de memoria, 16384, a los 108 del chunk anterior y los 8 bytes del *header* de ese chunk). Este valor esta apuntado por `sptr` en el diagrama anterior.
<br>La libreria inmediatamente se da cuenta del tamaño de la region libre, y entonces agrega el chunk libre de nuevo en la *free list*. Asumiendo que lo insertamos en el *head* de la *free list*, el espacio ahora luciria asi:

![](../Teorico-practico/imagenes/Insertado.png)

Tenemos una lista que empieza con un chunk libre pequeñ (100 bytes apuntados por el *head* de la lista) y un chunk libre grande (3764 bytes). Nuestra lista finalmente tiene mas de un element.
<br>Un ultimo ejemplo: Asumamos que los ultimos dos chunks que estan en uso son liberados. Sin fusion, terminariamos con fragmetacion:

![](../Teorico-practico/imagenes/Fragmentacion.png)

Como se puede ver, tenemos mucho desorden ya que nos olvidamos de **Fusionar** la lista. A pesar de que toda la memoria esta libre, esta cortado en piezas, por lo tanto aparenta ser una memoria fraccionada y no una sola.

#### Agregando el *Heap*

¿Que deberiamos hacer si el *heap* no alcanza para el espacio? El enfoque mas facil seria fallar. En algunos casos es la unica opcion, y por lo tanto retornar `NULL`.
<br>Los asignadores mas tradicionales inician con un *heap* chico y piden mas memoria al SO cuando la necesitan. Esto significa que hacen algun tipo de *system call* (por ejemplo, `sbrk`) para agrandar el *heap*, y entonces asignar los nuevos chunk ahi. Para satisfacer la peticion `sbrk`, el SO encuentra paginas fisicas libres en el *address space* del proceso que hizo la peticion, y entonces retorna el valor del final del nuevo *heap*; en este punto, un *heap* mas grande esta disponible, y la peticion puede ser atendida satisfactoriamente.

### Estategias Basicas

El asignador ideal es rapido y minima la fragmentacion. Pero, dado que la cadena de asignacion y liberaciones puede ser arbitraria, cualquier estrategia particular puede ser mala dado una mala sucesion de entradas.

1. #### *Best Fit*

Se busca a traves de la *free list* al bloque libre mas **Pequeño** de los espacios iguales o superiores al solicitados. Al hacer de una busqueda **Exhaustiva** en la *free list*, genera una penalizacion de *perfomance*, y una fragmentacion en espacios libres pequeños.

2. #### *Workst Fit*

Se busca al bloque mas **Grande** disponible, se usa el espacio necesario, y se devuelve lo restante a la *free list*. Genera los mismos ***Overheads*** al realizar tambien una busqueda **Exhaustiva** (fragmentando esta vez en bloques libres grandes).

3. #### *First Fit*

Usa el **Primer** bloque de la lista lo suficientemente grande para cumplir con lo solicitado. Su ventaja es la **Velocidad** ya que evita realizar una busqueda exhaustiva, pero "**Contamina** el **Comienzo** de la *free list* al concentrar alli la fragmentacion en bloques pequeños.

4. #### *Next Fit*

Igual que *First Fit* pero utiliza un puntero que le permite comenzar la busqueda desde la **Ultima Posicion** revisada la vez anterior. Desparrama la Fragmentacion a lo largo de la *free list* y mantiene la **Velocidad** del enfoque anterior, pero requiere un puntero extra en la implementacion.

#### Ejemplos

Supongamos una **free list** con tres elementos, de tamaño 10, 30 y 20 (se ignoran los *headers* y otros detalles):

![](../Teorico-practico/imagenes/FreeListEjemplos.png)

Asumamos una peticion de asignacion de tamaño 15. Un enfoque *Best Fit* buscaria en la lista entera y encontraria que 20 es el *Best Fit*, ya que es el menor espacio libre que puede servir para la peticion. El resulstao de la Free List:

![](../Teorico-practico/imagenes/BestFit.png)

Como se ve, queda un chunk libre pequeño. Un enfoque *Workst Fit* es similar pero en cambio busca el chunk mas grande, en el ejemplo seria 30. La lista resultante:

![](../Teorico-practico/imagenes/WorkstFit.png)

*First Fit*, en este ejemplo, hace lo mismo que *Workst Fit*, encontrar el primer bloque libre que pueda satisfacer la peticion. La diferencia es en el costo de la busqueda.

### Otros Enfoques

#### Listas Segregadas

Se utiliza en aplicaciones que tengan peticiones recurrentes de tamaño similar, se crea una lista para el manejo de objetos de ese tipi, y se envian las demas epticiones al *allocator* general. La fragmentacion es mejor y los pedidos de dicho tamaño se satisfacen rapido.
<br>Por ejemplo, el *Slab Allator* asigna un numero de *Object Caches* para objetos del kernel que se solicitan seguido. Si le falta espacio pide mas *slabs* (bloques pequeños) de memoria. Este *allocator* tambien mantiene los *free objects* de las listas en un estado pre inicializado.

#### *Buddy Allocation*

La memoria libre es pensada como un gran espacio de tamaño $2^{N}$. Caundo se hace una peticion de memoria, la busqueda de espacio libre recursivamente divide el espacio libre en dos hasta que encuentra un bloque suficientemente grande para acomodar la peticion. En Este punto, el bloque pedido es retornado al usuario. Un ejemplo de un espacio libre siendo dividio en la busqueda de un bloque de 7KB:

![](../Teorico-practico/imagenes/BloquesDivididos.png)

Se ve que el bloque de 8KB de mas a la izquierda es asignado y retornado al usuario; notar que este esquema sufre de **Fragmentacion Interna**, dado que solo puede dar bloques de tamaño multiplo de potencias de dos.
<br>Cuando un bloque se libera se chequea que su "*Buddy*" del mismo tamaño este libre, si lo esta los combina, y asi recursivamente hasta hacer *coalescing* de todo o encontrar un "*Buddy*" en uso; simplifica el *coalescing* pero genera Fragmentacion Interna.

## Capitulo 18: Introduccion a la Paginacion

El SO toma uno de dos enfoques para resolver casi cualquier problema de administracion de espacio. El primero es cortar el espacio en piezas de **tamaño variable**, como vimoes con la **Segmentacion**. Pero, esta solucion tiene problemas. En particular, cuando divide un espacio en chunks de diferentes tamaños, el mismo espacio se **Fragmenta**, y por lo tanto las asignaciones son mas dificiles.
<br>Por lo que, se debe considerar un segundo enfoque: Cortar el espacio en piezas de **tamaño fijo**. Esta idea es llamada **Paginacion**. En vez de dividir el *address space* de un proceso en segmentos logicos de tamaño variable (por ejemplo, *code*, *heap*, *stack*), lo dividimos en unidades de tamaño fijo, que las llamaremos **Paginas**. Vemos a la memoria fisica como un *array* de slots de tamaño fijo llamados ***Page Frames***; cada uno de esos *frames* puede contener una sola pagian de memoria virtual.

### Ejemplo y Descripcion General

![](../Teorico-practico/imagenes/EjemploPaginacion.png)

Se presenta un *address space* de 64 bytes de tamaño total, con cuatro paginas de 16 bytes.

![](../Teorico-practico/imagenes/MemoriaFisicaPaginacion.png)

Como vemos, la memoria fisica tambien contiene numero de *slots* de tamaño fijo, en este caso 8 *pages frames* (haciendo una memoria de 128 bytes). Las paginas del *addres space* virtual ha sido ubicado en diferentes lugares a lo largo de la memoria fisica; el diagrama tambien muestra el SO usando algo de memoria fisica para el mismo.

Las ventajas de la Paginacion son:
* **Flexibilidad**: Es capaz de apoyar la abstraccion del *address space* eficientemente, mas alla de como el proceso use el *address space* (por ejemplo, no se asume la direccion de crecimiento del *heap*/*stack* o como son usados).
* **Simplicidad**: Permite manejar el espacio libre (la *free list*) tan solo otorgando la cantidad de paginas necesarias (y evitando la fragmentacion externa). Tomando el ejempo anterior, cuando el SO quiere ubicar en el *address space* de 64 bytes en nuestra memoria fisica de 8 paginas, es facil encontrar 4 paginas libres; dado que el SO mantiene una *free list* de todas las paginas libres, el SO tiene ubicada la pagina virtual 0 del *address space* en el *frame* fisico 3, la pagina virtual 1 del *address space* en el *frame* fisico 7, la pagina 2 en el *frame* 5, y la pagina 3 en el frame 2. Los *pages frames* 1, 4 y 6 estan libres.

Para recordadr donde esta ubicada cada pagina virtual del *address space* en la memoria fisica, el SO mantiene una estructura de datos conocida como ***Page Table*** por proceso. El rol principal de la *page table* es guardar las traducciones de direcciones para cada pagina virtual del *address space*. En nuestro ejemplo, la *page table* deberia tener las siguientes cuatro entradas:

(*Virtual Page* 0 &rarr; *Physical Frame* 3), (VP 1 &rarr; PF 7), (VP 2 &rarr; PF 5), y (VP 3 &rarr; PF 2)

Imaginemos que el proceso con el **address space** anterior (64 bytes) esta haciendo un acceso a memoria.

```assembly
movl  <virtual address>,  %eax
```

Especificamente, prestemos atencion a la carga explicita de los datos de la direccion `<virtual address>` en el registro `eax`.
<br>Para **Traducir** esta direccion virtual que genero el proceso, primero tenemos que dividirla en dos componentes: El **Numero de Pagina Virtual** (***Virtual Page Number***) (**VPN**), y el **Offset** en la pagina. Para este ejemplo, dado que el *address space* es de 64 bytes, necesitamos 6 bits en total para nuestra direccion virtual ($2^{6}=64$). Por lo tanto, nuestra memoria virtual puede ser conceptualizada asi:

| Va5 | Va4 | Va3 | Va2 | Va1 | Va0 |
|:---:|:---:|:---:|:---:|:---:|:---:|

Va5 es el bit mas significativo de la direccion virtual, y Va0 es el bit menos significativo. Dado que sabemos el tamaño de la pagina (16 bytes), podemos dividir la direccion virtual asi:

| Va5 | Va4 | Va3 | Va2 | Va1 | Va0 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| VPN | VPN | *offset* | *offset* | *offset* | *offset* |

El tamaño de la pagina es de 16 bytes en un address space de 64 bytes; por lo que necesitamos ser capaces de seleccionar 4 paginas, y los primeros 2 bits de la direccion hacen justamente eso. Por lo tanto, tenemos una VPN de 2 bits. Los bits restantes nos dicen en que bytes de la pagina estamos interesados, 4 bits en este caso; esto lo llamos *offset*.
<br>Cuando un proceso genera una direccion virtual, el SO y el hardware deben comunicarse para traducirla en una direccion fisica. Por ejemplo, asumamos que la carga anterior es la direccion virtual 21:

```assembly
movl  21,  %eax
```

Pasanod 21 a binario, tenemos 010101, y por lo tanto podemos examinar esta direccion virtual y ver como se descompone en Numeros de Pagina Virtual (VPN) y *offset*:

| 0 | 1 | 0 | 1 | 0 | 1 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| VPN | VPN | *offset* | *offset* | *offset* | *offset* |

Por lo tanto, la direccion virtual 21 esta en el quinto byte (0101) de la pagina 01. Con nuestra VPN, podemos indexar nuestra *page table* y encontrar en que *frame* fisico recide la pagina virtual 1. En la *page table* de arriba el **Numero de Frame Fisico** (***Physical Frame Number***) (**PFN**) o **Numero de Pagina Fisica** (***Physical Page Number***) (**PPN**) es 7 (111). Por lo tanto, podemos traducir esta direccion virtual reemplazando el VPN con el PFN y entonces emitir la carga de memoria fisica.

![](../Teorico-practico/imagenes/TraduccionDeMemoria.png)

Notar que el *offset* se mantiene igual (o sea, no se traduce), ya que el *offset* solamente nos dique **que bit dentro de la pagina queremos**.

### ¿Donde se Guardan las *Pages Tables*?

Las *Pages Tables* son terriblemente grandes, por lo que no mantenemos ningun hardware especial en el chip en la MMU para guardar la *page table* de un proceso en ejecucion. En cambio, guardamos la *page table* de cada proceso en algun lugar de la memoria. Asumamos por ahora que las *page table* viven en la memoria fisica que administra el SO.

### ¿Que Hay Realmente en una *Page Table*?

Las *Pages Tables* son estructuras de datos usadas para mapear las direcciones virtuales a direcciones fisicas. Al ser muy largas y muchas (cada una tiene muchas ***Page Table Entry*** (**PTE**); una VPN de 20 bits, por ejemplo, implica $2^{20}$ traducciones) no se almacenan en la MMU, si no directamente en la memoria.

#### Organizacion
La mas simple es *Linear Page Table*; la *page table* es vista como un *array* el cual se indexa con el VPN y se busca el PTE (entrada en la *page table*) de dicha indexacion para encontrar el PFN.

Cada PTE tiene algunos bits importantes:
* ***Valid Bit***: Permite reservar *address space* al marcar paginas como invalidas, evitanto tener que asignarles memoria (por ejemplo, el espacio aun no solicitado entre el *code*/*heap* y el *stack* de un proceso al momento de crearlo).
* ***Protection Bits***: Indican si una pagina puede ser leida, escrita o ejecutada.
* ***Present Bit***: Indica si se encuentra en memoria fisica o en disco.
* ***Dirty Bit***: Indica si una pagina ha sido modificada desde que fue traida a memoria.
* ***Accessed Bit*** ("*Reference*"): Indica si la pagina ha sido accedida (util para determinar paginas populares que debe ser mantenida en memoria).

![](../Teorico-practico/imagenes/BitPageTable.png)
* En la imgaen se de la PTE, donde ***P*** = *Present Bit*, ***R/W*** = *Allowed Read/Write*, ***U/S*** = *Allowed Access to User-Mode Processes*, ***PWT/PCD/PAT/G*** = *Hardware Catching*, ***A*** = *Accesed*, ***D*** = *Dirty*, ***PFN***.

Notar que diferentes procesos (y diferentes VPN) pueden apuntar a una misma pagina (direccion) fisica (lo que reduce el numero de paginas fisicas en uso). Para esos casos, puede haber un bit ***G*** que indica si la pagina es compartida globalmente entre mas de un proceso.

Tanto el *Valid Bit* como los *Protection Bit* pueden lanzar una *trap* en caso de intentar acceder a la pagina cuando no se deberia poder hacerlo.

### Rapidez y Paginacion

La paginacion requiere que se realice una referencia a memoria extra para buscar la traduccion de la *page table* (de virtual a fisica, de la VPN a PTE y luego a PFN), lo cual es costoso, y dado el tamaño de las *pages tables*, se relentiza demasiado el sistema.
<br>Cuando corre, cada instruccion fetcheada genera dos referencias de memoria; una a la *page table* para encontrar el *Physical Frame* en la que la instruccion reside, y otra a la instruccion en si misma para poder fetchearla hacia la CPU.

## Capitulo 19: Traducciones Mas Rapidas: TLB