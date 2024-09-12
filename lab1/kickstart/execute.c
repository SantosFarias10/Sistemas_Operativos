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

        if (i < apipeLength - 1) {
            int res = pipe(pipefd);
            if (res < 0) {
                exit(EXIT_FAILURE);
            }
        }

        int forkResult = fork();                    //Creamos el hijo. Creo que va dentro de un ciclo donde se crean la cantidad de hijos necesarios, no solo uno

        if (forkResult < 0) {                       //Si falla fork()
            printf("FAILURE: fork()\n");
            exit(EXIT_FAILURE);
        }
        
        if (forkResult == 0) {               //El caso del hijo
            if (i > 0) {
                dup2(aux[0], STDIN_FILENO);
                close(aux[0]);
                close(aux[1]);
            }

            if (i < apipeLength - 1) {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            execute_scommand(pipeline_front(apipe));
            printf("FAILURE: execute_scommand() returned\n");
            exit(EXIT_FAILURE);
        } else {                                        //El caso del padre
            childenPIDS[i] = forkResult;

            if (i > 0) {                                //Cerramos el pipe previo
                close(aux[0]);
                close(aux[1]);
            }
            
            if (i < apipeLength - 1) {                  //Guardamos el pipe para el siguiente comando
                aux[0] = pipefd[0];
                aux[1] = pipefd[1];
            }
            
            pipeline_pop_front(apipe);
        }
    }

    if (apipeLength > 1) {          //Cerramos el ultimo pipe
        close(aux[0]);
        close(aux[1]);
    }
    
    if (pipeline_get_wait(apipe)) {
        for (unsigned int i = 0; i < apipeLength; i++) {
            int w = waitpid(childenPIDS[i], NULL, 0);
            
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
        unsigned int scommandLength = strlen(scommand_front(cmd)) + 1;
        argv[i] = malloc(scommandLength * sizeof(char));
        strcpy(argv[i], scommand_front(cmd));
        scommand_pop_front(cmd);
        i++;
    }
    argv[i] = NULL;   //El ultimo elemento, NULL

    char *in = scommand_get_redir_in(cmd);
    if (in != NULL) {                               //Si hay una redireccion in
        int resIN = open(in, O_RDONLY, S_IRWXU);    //Abrimos el path en Read-Only y con permisos de user owner
        if (resIN < 0) {
            printf("FAILURE: open()\n");
            exit(EXIT_FAILURE);
        }
        dup2(resIN, STDIN_FILENO);                  //Duplicamos el fd a STDIN
        close(resIN);                               //Cerramos el fd original
    }

    char *out = scommand_get_redir_out(cmd);
    if (out != NULL) {                              //Si hay una redireccion out
        int resOUT = open(out, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);  //Abrimos el path en Write-only, lo creamos si no existe, truncamos si existe
        if (resOUT < 0) {
            printf("FAILURE: open()\n");
            exit(EXIT_FAILURE);
        }
        dup2(resOUT, STDOUT_FILENO);                //Duplicamos el fd a STDOUT
        close(resOUT);                              //Cerramos el fd original
    }
    
    execvp(argv[0], argv);          //Ejecutamos el comando que esta en argv
    printf("FAILURE: execvp() returned, (%s)\n", argv[0]);
    exit(EXIT_FAILURE);
}
