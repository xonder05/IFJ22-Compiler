#pragma once
#include "dyn_string.h"

#define M_HEADER ".IFJcode22\nJUMP $$main\n"
#define M_MAIN "LABEL $$main\n"


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

    final_header_block,
    final_main_label_block,
    final_function_block,
    final_main_body_block,
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
void instListInsertLast( inst_list_t *list, block_type type, Dyn_String *code);
void instInsertBeforeWhile( inst_list_t *list, inst_list_elem_ptr elem);
void isntListDispose(inst_list_t *list);
void printList(inst_list_t *list);
void copyListToString(inst_list_t *list, Dyn_String *string);
void def_built_in(inst_list_t *list);

