#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "builtin.h"
#include "command.h"
#include "tests/syscall_mock.h"

const char * comandos[] = {
    "cd",
    "help",
    "exit"
};

const unsigned int cantidad_comandos = 3;


bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);
    bool result = false;

    char *commnd_name = scommand_front(cmd);

    unsigned int i = 0;
    while(!result && i < cantidad_comandos){
        if (strcmp(commnd_name, comandos[i]) == 0){
            result = true;
        }
        i++;        
    }

    return result;
}

bool builtin_alone(pipeline p){
    assert(p != NULL);
    
    bool result = false;

    if (pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p))){
        result = true;
    }

    return result;
}

void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd));

    char *cmd_name = scommand_front(cmd);

    if(strcmp(cmd_name, "cd") == 0){
        command_cd(cmd);
    }
    else if(strcmp(cmd_name, "help") == 0){
        command_help(cmd);
    }
    else if(strcmp(cmd_name, "exit") == 0){
        command_exit(cmd);
    }
}

//correr comando cd
void command_cd(scommand cd){
    assert(strcmp(scommand_front(cd), "cd") == 0);

    scommand_pop_front(cd);
    char *arg = scommand_front(cd);

    if(arg == NULL){
        arg = getenv("HOME");
    }

    int resCHDIR = chdir(arg);

    if (resCHDIR == -1){
        if (errno == ENOENT) {
            printf("Directorio no encontrado: %s\n", arg);
        } else if (errno == ENOTDIR) {
            printf("El path especificado no es un directorio: %s\n", arg);
        } else {
            perror("Error al cambiar de directorio");
        }
    }
}

//correr comando help
void command_help(scommand help){
    assert(strcmp(scommand_front(help), "help") == 0);

    printf("Bienvenido a nuestra shell MyBash\n");
    printf("Los autores son:\nAlejandro N. Pitt Aparicio\nSantino Ponchiardi\nBrandon Michel\nSantos Facundo Adrian Farias\n\n");
    printf("Los comandos a utilizar son:\n");
    printf("cd: Cambia el directorio actual\n");
    printf("help: Muestra el nombre de la shell, los autores y esta lista de comandos\n");
    printf("exit: Cierra la shell\n");
    printf("\n");
}

//correr comando exit
void command_exit(scommand _exit){
    assert(strcmp(scommand_front(_exit), "exit") == 0);
    exit(EXIT_SUCCESS);
}