# Proyecto de SHELL "MyBash" en C

Este proyecto implementa un shell básico en C llamado `mybash`, que incluye la capacidad de manejar pipelines y redirección de entrada/salida.

## Contenido

- [Instalación](#instalación)
- [Uso](#uso)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Contribución](#contribución)
- [Consideraciones](#consideraciones)

## Instalación

Para compilar y ejecutar este proyecto, necesitarás tener instalado un compilador de C (como `gcc`) y `make`.

1. Clona el repositorio:
    ```sh
    git clone https://github.com/tu-usuario/tu-repositorio.git
    cd tu-repositorio
    ```

2. Compila el proyecto, dentro del directorio kickstart:
    ```sh
    make
    ```

3. Ejecuta los tests general, dentro del directorio kickstart:
    ```sh
    make test
    ```

## Uso

Para ejecutar el shell `mybash`, simplemente ejecuta el binario compilado:

```sh
./mybash
```

Para ver la ayuda utilizar el comando `help`:

```sh
help
```

## Estructura del Proyecto

El proyecto `mybash` está estructurado de la siguiente manera:

```
so24lab1g36/
├── kickstart/
│   ├── Makefile
│   ├── builtin.c
│   ├── command.c
│   ├── execute.c
│   ├── execut.sh
│   ├── mybash.c
│   ├── obfuscated.c
│   ├── parsing.c
│   ├── strextra.c
│   ├── builtin.h
│   ├── command.h
│   ├── execute.h
│   ├── mybash.h
│   ├── obfuscated.h
│   ├── parsing.h
│   ├── strextra.h
│   ├── tests/
│   │   ├── Makefile
│   │   ├── leaktest.c
│   │   ├── run_tests.c
│   │   ├── syscall_mock.c
│   │   ├── test_execute.c
│   │   ├── test_parsing.c
│   │   ├── test_pipeline.c
│   │   ├── test_scommand.c
│   │   ├── syscall_mock.h
│   │   ├── test_execute.h
│   │   ├── test_parsing.h
│   │   ├── test_parser.h
│   │   ├── test_pipeline.h
│   │   ├── test_scommand.h
│   │   └── glib.supp
│   ├── objects-x86_64/
│   │   ├── lexer.o
│   │   └── parser.o
│   └── objects-i386/
│       ├── lexer.o
│       └── parser.o
├── extra/
│   ├── comandocopado.sh
│   └── ejemploMyBash.c
├── MyBash2024.pdf
└── README.md
```

- El directorio `kickstart` contiene los archivos fuente del shell `mybash`, incluyendo `builtin.c`, `command.c`, `execute.c`, `mybash.c`, `obfuscated.c`, `parsing.c`, `strextra.c`, `builtin.h`, `command.h`, `execute.h`, `mybash.h`, `obfuscated.h`, `parsing.h`, `strextra.h`.
- El directorio `tests` contiene los archivos de prueba para cada componente del shell, incluyendo `leaktest.c`, `run_tests.c`, `syscall_mock.c`, `test_execute.c`, `test_parsing.c`, `test_pipeline.c`, `test_scommand.c`, `syscall_mock.h`, `test_execute.h`, `test_parsing.h`, `test_parser.h`, `test_pipeline.h`, `test_scommand.h`, `glib.supp`.
- El archivo `Makefile` se utiliza para compilar y ejecutar el proyecto.
- El archivo `README.md` es el archivo de documentación que estás leyendo actualmente.

Esta estructura organizada facilita el mantenimiento y la comprensión del proyecto, separando claramente los componentes del shell y los archivos de prueba.

## Contribución

Participaron los estudiantes de la materia Sistemas Operativos, del grupo 36:
    `Santino Ponchiardi`
    `Santos Facundo Adrian Farias`
    `Brandon Michel`
    `Alejandro N. Pitt Aparicio`

## Consideraciones

1.  Intentamos encontrar cual es el causante de los memory leaks que tenemos, donde el principal problema es la funcion strdup(). Pero 12 de los 13 memory leaks
    pertenecen a los test, lo cual despues de leerlos y analizarlos no logramos encontrar el problema. Esto nos dejo estancados con el tema de la perdida de 
    memoria.

2.  Hay algunas funciones como `du /home/usuario | sort -n &` ó `ls | wc > out &` que tampoco pudimos encontrar la solución, ya que las dos parece que su problema
    es que al ejecutarse el `&` se quedan colgadas sin terminar lo que tienen que hacer, y en el caso del segundo comando, salta un error de wait(). Sentimos con
    el grupo que podriamos encontrar el problema, pero mientrastanto lo aclaramos por las dudas de no alcanzar a solucionarlo.