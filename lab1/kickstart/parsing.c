#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    /*if(p == NULL || parser_at_eof(p)){
        return NULL;
    }*/
    scommand result = scommand_new();
    arg_kind_t type;

    parser_skip_blanks(p);  //omitimos los espacios en blanco en el parser
    char *arg = parser_next_argument(p, &type); //obtenemos el siguiente argumento del parser y su tipo
    //char *arg;
    //si no hay argumentos, destruye el scommand y retornamos null
    if(arg == NULL){
        result = scommand_destroy(result);
        return NULL;
    }
    
    while(!parser_at_eof(p)){

        if (arg != NULL) {  //si hay argumentos
            if (type == ARG_OUTPUT) {   //si el argumento es del tipo redireccion de salida
                scommand_set_redir_out(result, arg);    //establecemos la redireccion de salida en el scommand
            } else if (type == ARG_INPUT) {     //si es del tipo redireccion de entrada
                scommand_set_redir_in(result, arg); // establece la redireccion de entrada en el scommand
            } else if (type == ARG_NORMAL) {    //si es del tipo normal
                scommand_push_back(result, arg);    //lo añadimos al scommand
            }
        } else if (type == ARG_INPUT || type == ARG_OUTPUT) {   //si no hay argumentos pero el tipo es de redirrecion
            return NULL;
        } else {    //si no hay argumento y el tipo no es de redirrecion, salimos del bucle
            break;
        }
        parser_skip_blanks(p);  //omitimos los espacios en blanco del parser
        arg = parser_next_argument(p, &type);   //obtenemos el siguiente argumento
    }

    return result;
}

pipeline parse_pipeline(Parser p) {
    assert(p != NULL && !parser_at_eof(p));
    
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool another_pipe = true;
    bool error = false;

    parser_skip_blanks(p);
    cmd = parse_scommand(p);    //analizamos el primer comando
    error = (cmd==NULL); /* Comando inválido al empezar */
    
    
    while (another_pipe && !parser_at_eof(p) && !error) {
        parser_skip_blanks(p);
        pipeline_push_back(result, cmd);    //añade el comando actual al pipeline
        parser_op_pipe(p, &another_pipe);   //verifica si hay un pipe y actualiza another_pipe
        
        if(another_pipe){   //si hay otro pipe
            cmd = parse_scommand(p);    //analiza el siguiente comando
            error = (cmd==NULL);
            if (error) {    //si hay error
                printf("FAILURE: no hay archivos input ni output\n");
                result = pipeline_destroy(result);
                return NULL;
            }
        }
    }
    
    bool bg;
    parser_op_background(p, &bg);   //verifica si hay un operador de segundo plano y actualiza
    if(bg){ //si hay operador de segundo plano
        pipeline_set_wait(result, false);   //establece que el pipeline no debe esperar a que los comandos terminen
    }

    bool garbage;
    parser_garbage(p, &garbage);    //verifica si hay caracteres no validos en el parser y actualiza el garbage
    if(garbage){    //si hay caracteres no validos
        printf("FAILURE: Comando Invalido\n");  
        result = pipeline_destroy(result);
    }   //su hay un error
    if(error){
        result = pipeline_destroy(result);
        return NULL;
    }

    return result;
}