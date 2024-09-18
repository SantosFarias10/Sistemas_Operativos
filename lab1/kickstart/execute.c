#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>

#include "execute.h"
#include "builtin.h"
#include "tests/syscall_mock.h"

void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);
    
    if (pipeline_is_empty(apipe)) {
        return;
    }

    if (builtin_is_internal(pipeline_front(apipe))) {                 //Es un comando interno, hacerlo correr
        execute_scommand(pipeline_front(apipe));
        return;
    } 

    if (!pipeline_get_wait(apipe)) {
        signal(SIGCHLD, SIG_IGN);
    }
    
    unsigned int apipeLength = pipeline_length(apipe);
    int *childenPIDS = malloc(apipeLength * sizeof(int));   //Para guardar los pids de los hijos
    int pipefd[2];
    int aux[2];

    for (unsigned int i = 0; i < apipeLength; i++) {

        //creamos un pipe 
        if (i < apipeLength - 1) {  //verificamos que i es menor a apipeLength-1, para asegurarnos de que no se intente crear mas pipe de los necesarios
            int res = pipe(pipefd); // crea un pipe y guarda los descriptors en "pipefd", pipefd[0] para lectura y pipe[1] para escritura
            if (res < 0) {  //pipe devuelve -1 si falla y 0 si tiene exito
                exit(EXIT_FAILURE);
            }
        }

        int forkResult = fork();                    //Creamos el hijo. Creo que va dentro de un ciclo donde se crean la cantidad de hijos necesarios, no solo uno   

        if (forkResult < 0) {                       //Si falla fork()
            printf("FAILURE: fork()\n");
            exit(EXIT_FAILURE);
        }
        
        if (forkResult == 0) {               //El caso del hijo
            //redireccion de entrada
            if (i > 0) {    //si i < 0 significa que no es el primer comando de la tuberia 
                dup2(aux[0], STDIN_FILENO); //redirige la entrada estandar (STDIN_FILENO) al extremo del pipe anterior (aux[0])
                //luego cierra ambos extremos del pipe anterior (aux[0] y aux[1])
                close(aux[0]);  
                close(aux[1]);
            }
            //redirreccion de salida 
            if (i < apipeLength - 1) {  // si i<apipeLenght-1 significa que no es el ultimo comando en la tuberia
                close(pipefd[0]);   // cierra el extrema de la lectura del pipe actual
                dup2(pipefd[1], STDOUT_FILENO); // redirige la salida estandar (STDOUT_FILENO) al extremo de escritura del pipe actual(pipefd[1])
                close(pipefd[1]);   //ciera el extremo de escritura del pipe actual
            }

            execute_scommand(pipeline_front(apipe));    //ejecuta el comando actual
            //si la ejecion falla imprime un mensaje de error y cierra el proceso hijo
            printf("FAILURE: execute_scommand() returned\n");   
            exit(EXIT_FAILURE);
        } else {                                        //El caso del padre
            childenPIDS[i] = forkResult;    //guardamos el PID del proceso hijo en el array childrenPIDS

            if (i > 0) {                                //Cerramos el pipe previo
                close(aux[0]);
                close(aux[1]);
            }
            
            if (i < apipeLength - 1) {                  //Guardamos los descriptors dle pipe actual en aux para que puedan ser usados por el siguiente comando en la pipeline
                aux[0] = pipefd[0];
                aux[1] = pipefd[1];
            }
            
            pipeline_pop_front(apipe);  //eliminamos el camando actual de la pipeline para que el siguiente comando pueda ser procesado
        }
    }
//Cerramos el ultimo pipe
    if (apipeLength > 1) {  //verificamos si hay mas de un comando en la pipeline          
        //cerramos ambos extremos del ultimo pipe
        close(aux[0]);
        close(aux[1]);
    }
    
    if (pipeline_get_wait(apipe)) { //verificamos si se debe esperar a que el proceso hijo termine
        for (unsigned int i = 0; i < apipeLength; i++) {    
            int w = waitpid(childenPIDS[i], NULL, 0);   //llamamos a waitpid para esperar a que cada proceso hijo termine
            
            if (w == -1) {                            //Si falla wait()
                printf("FAILURE: wait()\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    free(childenPIDS);
}

void execute_scommand(scommand cmd) {
    assert(cmd != NULL);
    
    if (builtin_is_internal(cmd)) {
        builtin_run(cmd);
        return;
    }

    unsigned int scommandSLength = scommand_length(cmd);
    char **argv = calloc(scommandSLength + 1, sizeof(char *)); //Reservamos memoria para los comandos. El +1 es para el ultimo caracter NULL

    unsigned int i = 0;
    while (!scommand_is_empty(cmd)) {                           //Llenamos el arreglo argv con el scommand y sus in/outd
        //llenamos el array argv con los argumentos del comando
        unsigned int scommandLength = strlen(scommand_front(cmd)) + 1;
        argv[i] = malloc(scommandLength * sizeof(char));
        strcpy(argv[i], scommand_front(cmd));
        scommand_pop_front(cmd);
        i++;
    }
    argv[i] = NULL;   //El ultimo elemento, NULL

    char *in = scommand_get_redir_in(cmd);  //obtenemos el archivo de redireccion de entrada
    if (in != NULL) {                               //Si hay una redireccion in
        int resIN = open(in, O_RDONLY, S_IRWXU);    //Abrimos el path en Read-Only y con permisos de user owner
        if (resIN < 0) {    //si falla imprimimos mensaje de error
            printf("FAILURE: open()\n");
            exit(EXIT_FAILURE);
        }
        dup2(resIN, STDIN_FILENO);                  //Duplicamos el fd a STDIN
        close(resIN);                               //Cerramos el fd original
    }

    char *out = scommand_get_redir_out(cmd);    //obtenemos el archivo de redireccion de salida
    if (out != NULL) {                              //Si hay una redireccion out
        int resOUT = open(out, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);  //Abrimos el path en Write-only, lo creamos si no existe, truncamos si existe
        if (resOUT < 0) {   //si falla imprimimos mensaje de error
            printf("FAILURE: open()\n");
            exit(EXIT_FAILURE);
        }
        dup2(resOUT, STDOUT_FILENO);                //Duplicamos el fd a STDOUT
        close(resOUT);                              //Cerramos el fd original
    }
    
    execvp(argv[0], argv);          //Ejecutamos el comando que esta en argv
    printf("FAILURE: execvp() returned, (%s)\n", argv[0]);  //si falla el execvp imprimimos un mensaje de error
    exit(EXIT_FAILURE);
}
