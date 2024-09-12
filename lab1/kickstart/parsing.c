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

    parser_skip_blanks(p);
    char *arg = parser_next_argument(p, &type);
    //char *arg;
    if(arg == NULL){
        result = scommand_destroy(result);
        return NULL;
    }
    
    while(!parser_at_eof(p)){

        if (arg != NULL) {
            if (type == ARG_OUTPUT) {
                scommand_set_redir_out(result, arg);
            } else if (type == ARG_INPUT) {
                scommand_set_redir_in(result, arg);
            } else if (type == ARG_NORMAL) {
                scommand_push_back(result, arg);
            }
        } else if (type == ARG_INPUT || type == ARG_OUTPUT) {
            return NULL;
        } else {
            break;
        }
        parser_skip_blanks(p);
        arg = parser_next_argument(p, &type);
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
    cmd = parse_scommand(p);
    error = (cmd==NULL); /* Comando inválido al empezar */
    
    
    while (another_pipe && !parser_at_eof(p) && !error) {
        parser_skip_blanks(p);
        pipeline_push_back(result, cmd);
        parser_op_pipe(p, &another_pipe);
        
        if(another_pipe){
            cmd = parse_scommand(p);
            error = (cmd==NULL);
            if (error) {
                printf("FAILURE: no hay archivos input ni output\n");
                result = pipeline_destroy(result);
                return NULL;
            }
        }
    }
    
    bool bg;
    parser_op_background(p, &bg);
    if(bg){
        pipeline_set_wait(result, false);
    }

    bool garbage;
    parser_garbage(p, &garbage);
    if(garbage){
        printf("FAILURE: Comando Invalido\n");
        result = pipeline_destroy(result);
    }
    if(error){
        result = pipeline_destroy(result);
        return NULL;
    }

    return result;
}