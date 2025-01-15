# Interludio: API de proceso

---

## System call `fork()`

La system call `fork()` es usada para crear un nuevo proceso.

`p1.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
    } else {
        // parent goes down this path (main)
        printf("hello, I am parent of %d (pid:%d)\n",
        rc, (int) getpid());
    }
    return 0;
}
```

```bash
prompt> ./p1
hello world (pid:29146)
hello, I am parent of 29147 (pid:29146)
hello, I am child (pid:29147)
prompt>
```

Cuando empieza a ejecutarse `p1.c` el proceso imprime un mensaje `"hello world"`; ademas en este mensaje hay un **Identificado de proceso** (process identifier), mas conocido como **PID**. El proceso tiene un PID de 29146; el PID es usado para nombrar a los procesos si uno quiere hacer algo con el.

El proceso llama a la system call `fork()`, la cual proporciona el SO como forma de crear un nuevo proceso. La parte rara es que el proceso creado es (casi) una copia exacta del proceso que lo creo. Esto quiere decir que para el SO ahora hay dos copias del programa `p1` ejecutandose, y ambos estan por regresar regresar de la system call `fork()`. El proceso recien creado (llamado **Hijo**), en contraste del proceso creador **Padre** no empieza a ejecutarse en el `main()`, este cobra vida como si el mismo hubiera llamado a `fork()`.

El hijo no es una copia exacta. Aunque ahora tiene su propia copia de espacio de direcciones (osea, su propia memoria privada), sus propios registros, su propio PC (Program counter), etc, el valor que retorna al creador de `fork()` es diferente. Mientras el padre recibe el PID del hijo recién creado, el hijo recibe código de retorno de cero. Esta diferencia es útil ya que hace simple escribir el código para manejar los dos casos diferentes.

La salida de `p1.c` no es deterministica. Cuando el proceso hijo es creado, ahora hay dos procesos activos en el sistema de los que preocuparnos. Asumimos que estamos ejecutando el programa en un sistema con un solo CPU, el padre o hijo podrian ejecutarse en ese punto. En el ejemplo se ve que se ejecuto el padre y por lo tanto imprimio el mensaje primero. Tambien pudo haber ocurrido al reves.

```bash
prompt> ./p1
hello world (pid:29146)
hello, I am child (pid:29147)
hello, I am parent of 29147 (pid:29146)
prompt>
```

El **planificador** del CPU (`scheduler`) es el que determina que proceso se ejecutara en un tiempo determinado.

---

## System Call `wait()`

A veces, resulta más útil que el padre espere a que el proceso hijo finalice lo que estaba haciendo. Esta tarea se logra con la system call `wait()` (o `waitpid()`).

`p2.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
    } else {
        // parent goes down this path (main)
        int rc_wait = wait(NULL);
        printf("hello, I am parent of %d (rc_wait:%d) (pid:%d)\n",
        rc, rc_wait, (int) getpid());
    }
    return 0;
}
```

```bash
prompt> ./p2
hello world (pid:29266)
hello, I am child (pid:29267)
hello, I am parent of 29267 (rc_wait:29267) (pid:29266)
prompt>
```

El proceso padre llama a `wait()` para retrasar su ejecución hasta que el hijo termine de ejecutarse. Cuando el hijo termina, `wait()` regresa al padre.

Agregando la llamada `wait()` en el código hace que la salida sea deterministica.

Sabemos que el hijo siempre imprimira primero, ya que si primero se ejecuta el padre, antes de imprimir llama a `wait()`; y la system call no retornara hasta que el hijo no finalice, y solo entonces continua su ejecución e imprime por pantalla.

---

## System call `exec()`

Esta system call es útil cuando queremos poner en ejecución un programa que es diferente del programa que lo llama. Osea, llamando a `fork()` en `p2.c` es solo útil si queremos mantener en ejecución copias del mismo programa. A menudo vamos a querer ejecutar un programa diferente y `exec()` hace justamente eso.

`p3.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  printf("hello world (pid:%d)\n", (int) getpid());
  int rc = fork();
  if (rc < 0) {
    // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) { // child (new process)
    printf("hello, I am child (pid:%d)\n", (int) getpid());
    char *myargs[3];
    myargs[0] = strdup("wc");
    // program: "wc" (word count)
    myargs[1] = strdup("p3.c"); // argument: file to count
    myargs[2] = NULL;
    // marks end of array
    execvp(myargs[0], myargs); // runs word count
    printf("this shouldn’t print out");
  } else {
    // parent goes down this path (main)
    int rc_wait = wait(NULL);
    printf("hello, I am parent of %d (rc_wait:%d) (pid:%d)\n",
    rc, rc_wait, (int) getpid());
  }
  return 0;
}
```

```bash
prompt> ./p3
hello world (pid:29383)
hello, I am child (pid:29384)
  29  107 1030  p3.c
hello, I am parent of 29384 (rc_wait:29384) (pid:29383)
prompt>
```

El proceso hijo llama a `execvp()` para ejecutar el programa `wc`, el cual es un programa contador de palabras. Ejecuta `wc` en el archivo fuente `p3.c`, por lo tanto nos dice cuantas lineas, palabras y bytes hay en un archivo.

Dado un nombre de un ejecutable (`wc`), y algunos argumentos (`p3.c`), carga el código y los datos estaticos de ese ejecutable y sobreescribe su segmento de código actual (y sus datos estaticos) con el nuevo programa; el heap y el stack y otras partes del espacio de memoria del programa son re-inicializadas. Entonces el SO simplemente ejecuta ese programa, pasandole todos los argumentos como el `argv` del proceso. Esta system call no crea un nuevo proceso; transforma el actual programa en ejecución (antes `p3`) en un diferente programa en ejecución (`wc`). Despues de la llamada a `exec()`, es como si `p3` nunca se hubiera ejecutado; una llamada exitosa a `exec()` nunca retorna.

---

## ¿Porque? Motivando la API

La separación de `fork()` y `exec()` es esencial para la construcción de una shell de UNIX, porque esto le permite al shell ejecutar código **despues** de la llamada `fork()` pero **antes** de la llamada `exec()`; este código puede alterar el ambiente del programa que esta a punto de ejecutarse, y por lo tanto habilitar una varidad de caracteristicas interesantes para ser construidos facilmente.

El shell es solo un programa de usuario, te muestra un **Prompt** y entonces espera a que tipees algo. Entonces tipeamos un comando (osea un programa ejecutable, mas algunos argumentos); en muchos casos, el shell decifra en que parte del **File System** se encuentra el ejecutable, llama a `fork()` para crear un nuevo proceso hijo para ejecutar el comando, llama a alguna variante de `exec()` para poner en ejecución el comando, y entonces espera que el comando complete su ejecución llamando a `wait()`. Cuando el hijo termina, el shell regresa del `wait()` e imprime de nuevo el **Prompt**, listo para el siguiente comando.

La separación de `fork()` y `exec()` le permite al shell hacer un puñado entero de cosas utiles mas facíl. Por ejemplo:

```bash
prompt> wc p3.c > newfile.txt
```

La salida del programa `wc` es redirigida en el archivo de salida *newfile.txt*. La forma en que el shell logra: Cuando el proceso hijo es creado, antes de llamar a `wait()`, el shell cierra la salida estandar y abre el archivo *newfile.txt*. Haciendo esto, Cualquier salida de `wc` a punto de ser ejecutado sera enviado al archivo en vez de la pantalla.

`p4.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  int rc = fork();
  if (rc < 0) {
    // fork failed
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) {
    // child: redirect standard output to a file
    close(STDOUT_FILENO);
    open("./p4.output", O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

    // now exec "wc"...
    char *myargs[3];
    myargs[0] = strdup("wc");
    // program: wc (word count)
    myargs[1] = strdup("p4.c"); // arg: file to count
    myargs[2] = NULL;
    // mark end of array
    execvp(myargs[0], myargs); // runs word count
  } else {
    // parent goes down this path (main)
    int rc_wait = wait(NULL);
  }
  return 0;
}
```

```bash
prompt> ./p4
prompt> cat p4.output
  32  109 846 p4.c
prompt>
```

La razón por la que la redirección funciona se debe a una suposición sobre como el SO maneja los **File Descriptors**. Los sistemas UNIX empiezan buscando file descriptors libres en cero. En este caso, *STDOUT_FILENO* sera el primero en estar disponible y por lo tanto es asignado cuando `open()` es llamado. Subsecuentemente escrito por el proceso hijo en la salida estandar del file descriptor.

Cuando se ejecuta `p4.c`, es como si nada hubiera pasado; el shell solo imprime el command prompt e inmediatamente esta listo para el siguiente comando. Pero, este no es el caso, de hecho el programa `p4.c` llamo a `fork()` para crear un nuevo hijo, y entonces ejecuto el programa `wc` a travez de una llamada a `execvp()`. No se ve impresa ninguna salida en la pantalla porque a sido redireccionada al arcivo *p3.output*. Cuando ejecutamos `cat` con el archivo de salida, se imprime toda la salida esperada del `wc`.

Los **Pipes** de UNIX son implementados de una forma similar, pero con la system call `pipe()`. En este caso, la salida de uno de los procesos es conectada a un **in-kernel pipe**, osea una cola, y la entrada de otro proceso es conectada al mismo pipe; la salida de un proceso aparenta ser usada como la entrada del siguiente, y una larga y útil cadena de comandos pueden ser insertadas juntas. En el ejemplo, consideramos buscar una palabra en un archivo, y contar cuantas veces esta esa palabra; los pipes y las utilidades `grep` y `wc`; solo hay que tipear `grep -o foo file | wc -l` en el command prompt.

---

## Usuario y control de procesos

Ademas de `fork()`, `exec()` y `wait()`, en los sistemas UNIX existen otras formas de interactuar con los procesos. La system call `kill()` es usada para enviar señales a un proceso, incluyendo directivas como pausar, morir y otros imperativos útiles. En muchas shell de UNIX, ciertas combinaciones de teclas son especificadas para llevar señales especificas al actual proceso en ejecución; *Control-C* envia una **SIGINT** (interrumpir) al proceso (normalmente terminandolo) y *Control-Z* envia una señal **SIGSTP** (stop) pausando el proceso en el medio de la ejecución.

El subsistema completo de señales proporciona una insfractutura para enviar eventos externos a los procesos, incluyendo formas de recibir y procesar esas señales en procesos individuales, y formas para enviar señales tanto a proceso individuales como a procesos grupales. Para usar esta forma de comunicación, un proceso debe usar la sytem call `signal()` para "agarrar" varias señales; haciendolo se asegura que cuando una señal particular es entregada a un proceso, este suspendera su ejecución normal y ejecutara una parte particular de código en respuestas de la señal.

##### ¿Quien puede enviar una señal a un proceso, y quien no?

Los sistemas que usamos pueden tener multiples personas usandolos al mismo tiempo; si una de esas personas puede arbitrariamente enviar señales como *SIGINT* para interrumpir el proceso, la usabilidad y seguridad del sistema estaria comprometida. Como resultado, los sistemas modernos incluyen una fuerte concepción de la noción de **Usuario**. El usuario, depues de ingresar una contraseña para establecer credenciales, inicia sesión para ganar accesos a los recursos del sistema. Entonces el usuario puede ejecutar uno o muchos procesos, y ejerce control total sobre ellos (pausarlo, matarlos, etc). Los usuarios generalmente solo pueden controla sus propios procesos; el trabajo del SO es repartir recursos a cada usuario para cumplir con los objetivos generales del sistema.

---

## Herramientas utiles

Usando el comando `ps` podemos ver que proceso se estan ejecutando; leer la **Man Page** para ver algunas banderas (flags) utiles para pasarle a `ps`. `top` muestra por pantalla los procesos del sistema y cuanta CPU y otros recursos estan usando. Muchas veces cuando ejecutamos este comando, dice que el proceso que mas recursos consume; quizas eso sea algo egolatra. `kill` puede ser usado para enviar arbitrariamente señales a procesos, y uno levemente mas amistoso `killall`. 