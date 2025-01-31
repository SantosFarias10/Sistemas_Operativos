# Interludio: API de memoria

---

## Tipos de memoria

En un programa en ejecución en C, hay dos tipos de memoria asignada. La primera es llamada **Stack**, y las asignaciones y desasignaciones son manejadas *implicitamente* por el compilador para ti, el programador; por esta razon es llamada memoria *automatica*.

Declarar memoria en el stack en C es facil. Por ejemplo, digamos que necesitamos algo de espacio en la función `func()` para un entero llamado `x`. Para declar una pieza de memoria, solo necesitamos hacer algo como:

```c
void func(){
    int x; /* Declara un número entero en la pila (stack) */
}
```

El compilador hace el resto, asegurandose de hacer espacio en el stack cuando llames a `func()`. Cuando vuelves de la función, el compilador desasigna la memoria por ti; si queremos que alguna información sobreviva mas alla de la invocación, mejor no dejes esa información en el stack xd.

Es esta necesidad de memoria de larga vida que nos lleva al segundo tipo de memoria, llamado **heap**, donde todas las asignaciones y desasignaciones son manejadas por ti. Una gran responsabilidad y ciertamente la causa de muchos bugs. Pero si sos cuidadoso y prestas atención, usaras esas interfaces correctamente y sin demasiado problema. Ejemplo de como se debe asignar memoria para un entero en el heap.

```c
void func(){
    int *x = (int *) malloc(sizeof(int));
}
```

Un par de notas. Notar que ambas asignaciones, en el stack y heap, ocurren en esta linea: primero el compilador sabe hacer espacio para un puntero a un entero cuando ve la declaración a dicho puntero `(int *)`; subsecuentemente, cuando el programa llama a `malloc()`, pide memoria para un entero en el heap; la rutina devuelve la dirección de dicho entero (ya sea exitoso, o `NULL` en caso de fallar), el cual es guardado en el stack para ser usado por el programador.

Dada su naturaleza explicita, y dado su uso mas variado, la memoria heap presenta mas reto para ambos, usuario y sistema.

---

## La llamada `malloc()`

La llamada `malloc()`: consulta por espacio en el heap pasando el tamaño, y en cualquier caso te devuelve un puntero, al nuevo espacio asignado, si falla devuelve `NULL`.

La pagina del manual te muestra que necesitas para usar `malloc`; tenes que tipear `man malloc` en la linea de comandos y veras:

```bash
#include <stdlib.h>
...
void *malloc(size_t size);
```

De esta información, puedes ver que todo lo que necesitas hacer es incluir el archivo encabezado `stdlib.h` para usar `malloc`. Realmente no necesitas hacerlo, como la libreria C, a la cual todos los programas estan enlazados por defecto, tiene adentro el código para `malloc()`; agrega el encabezado solo le permite al compilador verificar que estes llamando a `malloc()` correctamente (osea, que estes pasando el numero correcto de argumentos, del tipo correcto, etc).

El único parametro que toma `malloc()` es de tipo `size_t` el cual solamente describe cuantos **bytes** necesitas. Sin embargo, la mayoria de los programadores no tipean directamente un número; ya que es una pobre forma de hacerlo. Se usan muchas rutinas y macros para hacerlo. Por ejemplo, para asignar espacio para un valor flotante de doble precisión (`double`):

```c
double *d = (double *) malloc(sizeof(double));
```

Esta inovocación a `malloc()` usa el operador `sizeof()` para solicitar la cantidad correcta de espacio; en C, esto suele pensarse como un operador en *tiempo de ejecución*, lo que significa que el tamaño real se conoce *al momento de compilar* y por lo tanto un número (en este caso 8, tamaño para un `double`) es sustituido como argumento en `malloc()`. Por esto `sizeof()` es pensado como un operador y no como una llamada a una función (una llamada a una función toma lugar en tiempo de ejecución).

También puedes pasarle el nombre de una variable (y no solo el tipo) a `sizeof()`, pero en algunos casos podrias no obtener los resultados esperados, entonces tenes que ser cuidadoso. Por ejemplo, vemos el siguiente código:

```c
int *x = malloc(10 * sizeof(int));
printf("%d\n", sizeof(x));
```

Declaramos espacio para un array de 10 integers. Cuando usamos `sizeof()` en la siguiente linea, devuelve un número chico, algo como 4 (en máquinas de 32 bits) u 8 (en máquinas de 64 bits). La razon es que `sizeof()` cree que le estamos preguntando por el tamaño de un *puntero* a un entero. no sobre cuanta memoria tenemos asignada dinamicamente. A veces, `sizeof()` funciona como se espera:

```c
int x[10];
printf("%d\n", sizeof(x));
```

En este caso, hay suficiente información estatica para que el compilador sepa que fueron asignados 40 bytes.

Otro caso con el que hay que tener cuidado, es con los `strings`. Cuando declaramos espacio para un `string`, usamos el siguiente formato: `malloc(strlen(s) + 1)`, el cual obtiene el largo del string usando la función `strlen()`, y le sumamos uno para hcer espacio para el caracter de fin del `string`. Usando `sizeof()` podria ser para problemas.

Notar que `malloc()` devuelve un puntero a `void`. Hacer esto es la forma que tiene C para devolver una dirección y dejar que el programador decida qué hacer con ella. El programador ademas se ayuda con lo que llamamos un **casteo** (**cast**); en el ejemplo anterior, el programador castea el tipo que devuelve `malloc()` a un puntero a `double`. Castear no es realmente complicado, ademas de decirle al compilador y a otros programadores "Si, es lo que estoy haciendo". Castea el resultado de `malloc()`, aunque no sea necesario, da seguridad.

---

## La llamada `free()`

Para liberar memoria del heap no es largo de hacer, los programadores simplemente llaman a `free()`:

```C
int *x = malloc(10 * sizeof(int));
...
free(x);
```

Toma como argumento un puntero retornado por `malloc()`. Notar que el tamaño de la región asignada no es pasada por el usuario, y debe ser rastreada por la misma libreria de asignación de memoria.

---

## Errores comunes

Hay ciertos errores que surgen con el uso de `malloc()` y `free()`. Los siguientes ejemplos compilaron pero el compilador no encontro estos errores.

* ### Olvidar asignar memoria

Muchas rutinas esperan memoria para ser aignada antes de que la llames. Por ejemplo, la rutina `strcpy(dst, src)` copia un `string` de un puntero fuente a un puntero destino. Pero, si no tenes cuidado podrias hacer algo como:

```c
char *src = "Hello";
char *dst; //ops! No asignado
strcpy(dst, src); // falla de segmentación y muere xd
```

Cuando ejecutas este código, probablemente conducira a un **Segmentation Fault** (**Falla de Segmentación**), el cual es un termino lindo para saber que **HICISTE ALGO MAL CON LA MEMORIA**.

En este caso, el código apropiado deberia ser algo como:

```c
char *src = "Hello";
char *dst = (char *) malloc(strlen(src) + 1);
strcpy(dst, src); //work properly
```

Como una alternativa podrias usar `strdup()` y hacerte la vida mas facil.

* ### No asignar suficiente memoria

Los errores relacionados con no asignar suficiente memoria a veces son llamados **Buffer Overflow**. En el ejemplo anterior, un error común es asignar *casi* el espacio suficiente para el buffer destino.

```c
char *src = "Hello";
char *dst = (char *) malloc(strlen(src)); //to small!!
strcpy(dst, src); //work properly
```

Dependiendo de como este implementado `malloc()` y muchos otros detalles, este programa podria ejecutarse aparentemente correctamente. En algunos casos, cuando se ejecuta la copia del `string`, escribe un byte mas lejos del final del espacio asignado, en algunos casos es inofensivo, sobreescribiendo algunas variables que ya no se usa. En algunos casos, este **overflow** puede ser increiblemente dañino, y de hecho es la fuente de vulnerabilidades de seguridad en sistemas. En otros casos, la libreria de asignación asigna un poco de espacio extra de todos modos, y por lo tanto tu programa realmente no sobreescribira otros valores de variables y funcionara bien. Incluso en otros casos, el programa en efecto fallara y se rompera.

* ### Olvidar inicializar la memoria asignada

Con este error, podes llamar a `malloc()` de forma apropiada, pero olvidar de llenar tu tipo de dato recién asignado con un valor. Si te olvidas de hacerlo, eventualmente tu programa encontrara una **lectura sin asignar** cuando lea desde el heap algun dato de valor desconocido.

* ### Olvidar liberar memoria

Las **memory leak**, y esto ocurre cuando olvidas de liberar memoria. En los sistemas o aplicaciones de larga ejecución es un gran problema, y no liberar memoria lentamente llevara a que te quedes sin memoria, en el que sera necesario reiniciar la máquina. Por eso, cuando terminas de usar un bloque de memoria asegurate de liberarlo.

En algunos casos, parece razonable no liberar memoria. Por ejemplo, si tu programa es corto, y termina pronto; en este caso, cuando el programa finalice, el SO limpiara todas las paginas asignadas y no habra memory leak en si. Mas alla de que esto funcione, es un mal habito. A la larga, uno de los objetivos como desarrollador es adquirir buenos habitos.

* ### Liberar memoria antes de terminar de usarla

Tal error se conoce como **Puntero colgado** (**Dangling pointer**). Y su uso puede bloquear el programa, o sobreescribir memoria valida (por ejemplo, llamaste a `free()` y despues a `malloc()` que reciclara la memoria recién liberada).

* ### Liberar memoria repetidamente

Es conocido como **Double Free**. El resultado de hacer esto esta indefinido. La libreria de asignación de memoria prodria confundirse y hacer todo tipo de cosas raras; lo mas común es que se creashee.

* ### Llamar a `free()` de forma incorrecta

Despues de todo, `free()` espera que le pases uno de los punteros que reciviste de un `malloc()`. Cuando le pasas algun otro valor, cosas malas pueden pasar. Ese **Acceso Invalido** (**Invalid acces**) es peligroso y obviamente debe ser evitado.

---

## Soporte subyacente del SO

Notar, hemos estado hablando de system calls cuando discutimos `malloc()` y `free()`. La razón es simple: no son system calls pero si son librery calls. La libreria de `malloc()` maneja espacio en tu espacio de direcciones virtuales, pero en si misma estan construidads en lo alto de algunas system calls las cuales llaman al SO ṕara pedir mas memoria o liberarla.

Dicha system call es llamada `brk`, la cual es usada para cambiar la locación de quiebre del programa: la ubicación del final del heap. Toma un argumento (la dirección del nuevo final), e incrementa o decrementa el tamaño del heap basado en si el nuevo final es mas grande o corto que el actual.

Notar que nunca vas a usar directamente `brk`. Estas son usadas por la libreria de asignación de memoria; si las intentas usar, probablemente haras algo horriblemente mal xd.

También puedes obtener memoria del SO a traves de la llamada `mmap()`. Pasandole los parametros correctos, `mmap()` puede crear una region de memoria **anonima** en tu programa. Esta memoria puede ser tratada como un heap y manejada como tal.

---

## Otras llamadas

`calloc()` asigna memoria y las inicia en cero antes de retornarla; esto previene algunos errores cuando asumes que la memoria contiene ceros y te olvidas de inicializarla. La rutina `realloc()` reserva una region de memoria nueva mas grande, copia el contenido de la region vieja, y retorna el puntero a la nueva region.