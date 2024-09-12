#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <glib.h>       /*para tener las estructuras de datos que ofrece GLib*/

#include "command.h"
#include "strextra.h"
#include "tests/syscall_mock.h"

struct scommand_s {
    GQueue *queue;      /* Ya tenemos la longitud de la cola en esta estructura */
    char *out;
    char *in;
};

scommand scommand_new(void) {
    scommand result = malloc(sizeof(struct scommand_s));
    assert(result != NULL);
    
    result->queue = g_queue_new();
    result->out = NULL;
    result->in = NULL;
    
    assert(result != NULL && scommand_is_empty(result) && scommand_get_redir_in(result) == NULL && scommand_get_redir_out(result) == NULL);
    return result;
}

scommand scommand_destroy(scommand self) {
    assert(self != NULL);
    g_queue_free(self->queue);
    self->queue = NULL;
    free(self->out);
    free(self->in);
    self->out = NULL;
    self->in = NULL;
    free(self);
    self = NULL;
    
    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument) {
    assert(self != NULL && argument != NULL);
    
    g_queue_push_tail(self->queue, argument);
    
    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self) {
    assert(self != NULL && !scommand_is_empty(self));
    
    char *argument = (char *) g_queue_pop_head(self->queue);
    free(argument);
}

void scommand_set_redir_in(scommand self, char * filename) {
    assert(self != NULL);
 
    free(self->in);
    self->in = filename;
}

void scommand_set_redir_out(scommand self, char * filename) {
    assert(self != NULL);

    free(self->out);
    self->out = filename;
}

bool scommand_is_empty(const scommand self) {
    assert(self != NULL);

    return (g_queue_is_empty(self->queue));
}

unsigned int scommand_length(const scommand self) {
    assert(self != NULL);
    
    if (g_queue_is_empty(self->queue)) {
        return 0;
    }
    unsigned int length = (unsigned int) g_queue_get_length(self->queue);
    
    assert((length == 0) == scommand_is_empty(self));
    return length;
}

char* scommand_front(const scommand self) {
    assert(self != NULL && !scommand_is_empty(self));
    
    char* front = g_queue_peek_head(self->queue);   
    
    assert(front != NULL);
    return front;
}

char* scommand_get_redir_in(const scommand self) {
    assert(self != NULL);
    return self->in;
}

char* scommand_get_redir_out(const scommand self) {
    assert(self != NULL);
    return self->out;
}

char* scommand_to_string(const scommand self) {
    assert(self != NULL);
    
    GQueue *command = g_queue_copy(self->queue);
    char* result = NULL;

    while (!g_queue_is_empty(command)){
        char *head = g_queue_pop_head(command);
        if (result == NULL){
            result = malloc(strlen(head) + 1);
            strcpy(result, head);
        } else {
            char *esp = strmerge(result, " ");
            free(result);
            result = strmerge(esp, head);
            free(esp);
        }
    }

    if (result != NULL && strlen(result) > 0 && result[strlen(result) - 1] == ' ') {
        result[strlen(result) - 1] = '\0';
    }

    /* Agregamos los redirectores */
    if (self->in != NULL){
        char *esp = strmerge(result, " < ");
        free(result);
        result = strmerge(esp, self->in);
        free(esp);
    }

    if (self->out != NULL){
        char *esp = strmerge(result, " > ");
        free(result);
        result = strmerge(esp, self->out);
        free(esp);
    }

    if (result == NULL) {
        result = strdup("");
    }
    
    g_queue_free(command);
    assert(scommand_is_empty(self) || scommand_get_redir_in(self) == NULL || scommand_get_redir_out(self) == NULL || strlen(result)>0);

    return result;
}

struct pipeline_s {
    GQueue *scommands;
    bool fg;
};

pipeline pipeline_new(void){
    pipeline result = malloc(sizeof(struct pipeline_s));
    assert(result != NULL);
    
    result->scommands = g_queue_new();
    result->fg = true;      /* Por defecto, el pipeline espera */
    
    assert(result != NULL && pipeline_is_empty(result) && pipeline_get_wait(result));
    return result;
}

pipeline pipeline_destroy(pipeline self){
    assert(self != NULL);
    
    while (!pipeline_is_empty(self)) {
        pipeline_pop_front(self);
    }
    free(self);
    self = NULL;
    
    assert(self == NULL);
    return self;
}

void pipeline_push_back(pipeline self, scommand sc) {
    assert(self != NULL && sc != NULL);

    g_queue_push_tail(self->scommands, sc);
    
    assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self) {
    assert(self != NULL && !pipeline_is_empty(self));
    
    scommand_destroy(g_queue_pop_head(self->scommands));
    
}

void pipeline_set_wait(pipeline self, const bool w) {
    assert(self != NULL);
    self->fg = w;
}

bool pipeline_is_empty(const pipeline self) {
    assert(self != NULL);
    return g_queue_is_empty(self->scommands);
}

unsigned int pipeline_length(const pipeline self) {
    assert(self != NULL);
    
    unsigned int length = (unsigned int) g_queue_get_length(self->scommands);
    
    assert((length == 0) == pipeline_is_empty(self));
    return length;
}

scommand pipeline_front(const pipeline self) {
    assert(self != NULL && !pipeline_is_empty(self));
    
    scommand sc = g_queue_peek_head(self->scommands);
    
    assert(sc != NULL);
    return sc;
}

bool pipeline_get_wait(const pipeline self) {
    assert(self != NULL);
    return self->fg;
}

char* pipeline_to_string(const pipeline self) {
    assert(self != NULL);
    
    GQueue *command = g_queue_copy(self->scommands);
    char* result = NULL;

    while (!g_queue_is_empty(command)){
        scommand head = g_queue_pop_head(command);
        if (result == NULL){
            result = scommand_to_string(head);
        } else {
            char* esp = strmerge(result, " | ");
            free(result);
            result = strmerge(esp, scommand_to_string(head));
            free(esp);
        }
    }
    
    if (!pipeline_get_wait(self)){
        char* esp = strmerge(result, " &");
        free(result);
        result = strmerge(esp, " ");
        free(esp);
    }
    
    if (result != NULL && strlen(result) > 0 && result[strlen(result) - 1] == ' ') {
        result[strlen(result) - 1] = '\0';
    }

    if(result == NULL){
        result = strdup("");
    }
 
    g_queue_free(command);
    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(result)>0);
    return result;
}