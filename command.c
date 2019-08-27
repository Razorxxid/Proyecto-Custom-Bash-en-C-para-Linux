#include <stdlib.h>
#include <stdbool.h>
#include <gmodule.h>
#include <assert.h>
#include "command.h"
#include "./bstring/bstrlib.h"


struct scommand_s{
    GQueue *queue;
    bstring input;
    bstring output;
};


scommand scommand_new(void){
    scommand result=calloc(1, sizeof(struct scommand_s));
    assert(result!=NULL && scommand_is_empty(result));
    result->queue=g_queue_new(); 
    result->input=NULL;
    result->output=NULL;
    return(result);
}

scommand scommand_destroy(scommand self){
    assert(self!=NULL);
    if(self!=NULL){
        g_queue_free(self->queue);
        bdestroy(self->input);
        bdestroy(self->output);
        free(self);
	}
    return NULL;
}


void scommand_push_back(scommand self, bstring argument){
    assert(self!=NULL && argument!=NULL);
    g_queue_push_tail(self->queue, argument);
}

bool scommand_is_empty(const scommand self){
    assert(self!=NULL);
    return (g_queue_is_empty (self->queue));

}
void scommand_pop_front(scommand self){
    assert(self != NULL && !scommand_is_empty(self));
    g_queue_pop_head(self->queue);
}

void scommand_set_redir_in(scommand self, bstring filename){
    assert(self!=NULL);
    self->input=filename;
}

void scommand_set_redir_out(scommand self, bstring filename){
    assert(self!=NULL);
    self->output=filename;
}

unsigned int scommand_length(const scommand self){
    assert(self!=NULL);
    return(g_queue_get_length(self->queue));
}

const_bstring scommand_front(const scommand self){
    assert(self!=NULL && !scommand_is_empty(self));
    return (g_queue_peek_head(self->queue));
}

const_bstring scommand_get_redir_in(const scommand self){
    assert(self!=NULL);
    return(self->input);
}

const_bstring scommand_get_redir_out(const scommand self){
    assert(self!=NULL);
    return(self->output);
}


bstring scommand_to_string(const scommand self){
    GList *list;
    bstring str=NULL;
    assert(self!=NULL);
    if (!scommand_is_empty(self)) {
        if (scommand_get_redir_in(self)==NULL && scommand_get_redir_out(self)==NULL) {
            list=self->queue->head;
            str=bstrcpy(list->data);
            for (unsigned int i=0u; i<scommand_length(self)-1u; ++i) {
                bconcat(str, bfromcstr (" "));
                list=list->next;
                bconcat(str, bstrcpy(list->data));
            }
        }
        else if (scommand_get_redir_in(self)!=NULL && scommand_get_redir_out(self)==NULL) {
            list=self->queue->head;
            str=bstrcpy(list->data);
            for (unsigned int i=0u; i<scommand_length(self)-1u; ++i) {
                bconcat(str, bfromcstr ("  "));
                list=list->next;
                bconcat(str, bstrcpy(list->data));
            }
            bconcat(str, bfromcstr(" < "));
            bconcat(str, scommand_get_redir_in(self));
        }
        else if (scommand_get_redir_in(self)==NULL && scommand_get_redir_out(self)!=NULL) {
            list=self->queue->head;
            str=bstrcpy(list->data);
            for (unsigned int i=0u; i<scommand_length(self)-1u; ++i) {
                bconcat(str, bfromcstr (" "));
                list=list->next;
                bconcat(str, bstrcpy(list->data));
            }
            bconcat(str, bfromcstr(" > "));
            bconcat(str, scommand_get_redir_out(self));    
        } 
        else {
            list=self->queue->head;
            str=bstrcpy(list->data);
            for (unsigned int i=0u; i<scommand_length(self)-1; ++i) {
                bconcat(str, bfromcstr (" "));
                list=list->next;
                bconcat(str, bstrcpy(list->data));
            }
            bconcat(str, bfromcstr(" < "));
            bconcat(str, scommand_get_redir_in(self));
            bconcat(str, bfromcstr(" > "));
            bconcat(str, scommand_get_redir_out(self));
        }
    } 
    return(str);
}

struct pipeline_s{
    GQueue *queue;
    bool h_wait;
};

pipeline pipeline_new(void) {
    pipeline self=calloc(1, sizeof(struct pipeline_s));
    self->queue=g_queue_new(); 
    assert(self!=NULL);
    self->h_wait=FALSE;
    return self;
}

pipeline pipeline_destroy(pipeline self) {
    assert(self!=NULL);
   
    g_queue_free(self->queue);
    free(self);
    
    return(NULL);
}

void pipeline_push_back(pipeline self, scommand sc) {
    assert(self!=NULL && sc!=NULL);
    g_queue_push_tail(self->queue, sc);  
}

void pipeline_pop_front(pipeline self){
    assert(self!=NULL && !pipeline_is_empty(self));
    g_queue_pop_head(self->queue);
}

void pipeline_set_wait(pipeline self, const bool w) {
    assert(self!=NULL);
    self->h_wait=w;
}

bool pipeline_is_empty(const pipeline self){
    assert(self!=NULL);
    return (pipeline_length(self)==0);
}
    

unsigned int pipeline_length(const pipeline self){
    assert(self!=NULL);
    return (g_queue_get_length(self->queue));
}
    
scommand pipeline_front(const pipeline self){
    assert(self!=NULL && !pipeline_is_empty(self));
    return (g_queue_peek_head(self->queue)) ;
}

bool pipeline_get_wait(const pipeline self){
    assert(self!=NULL);
    return (self->h_wait);
}
bstring pipeline_to_string(const pipeline self){
    GList *list;
    bstring new=NULL;
    assert(self!=NULL);
    if(!pipeline_is_empty(self)) {
        list=self->queue->head;
        new=scommand_to_string(list->data);
        for (unsigned int i=0u; i<pipeline_length(self)-1; ++i) {
            list=list->next;
            bconcat(new, bfromcstr (" | "));
            bconcat(new, scommand_to_string(list->data));       
        }
        if(pipeline_get_wait(self)==FALSE) {
            bconcat(new, bfromcstr (" & "));
        }
    }
    return(new);
}