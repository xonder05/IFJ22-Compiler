#pragma once
#include "dyn_string.h"

typedef enum
{
    while_loop_start,
    while_loop_end,
    while_loop_body,
    func_beg,
    func_body,
    func_end,
    func_call,
    var_dec,
    var_def,
    statement,
}block_type;


typedef struct inst_list_elem{
    block_type type;
    Dyn_String *code;
    struct inst_list_elem *next;
    struct inst_list_elem *previous;
}*inst_list_elem_ptr ;


typedef struct{
    inst_list_elem_ptr first;
    inst_list_elem_ptr last;
} inst_list_t;



inst_list_elem_ptr create_elem(block_type type,Dyn_String *code);
void instListInit(inst_list_t *list);
void instListInsertLast( inst_list_t *list, inst_list_elem_ptr elem);
void instInsertBeforeWhile( inst_list_t *list, inst_list_elem_ptr elem);
void isntListDispose(inst_list_t *list);

