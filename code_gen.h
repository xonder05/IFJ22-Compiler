/*************************************************
* 
*   file name: code_gen.h
*   VUT FIT Project Compiler for language IFJ22
*   Author: Ondřej Bahounek xbahou00
*   Description: Code generating from syntax tree
*
*************************************************/

#pragma once
#include "scanner.h"
#include "abstact_syntax_tree.h"
#include "code_gen_build.h"

//parametr strom
int generate(ast_t *tree);

//parametry strom, main_list,func_list
int gen_from_ass(ast_t *tree,inst_list_t *body_list,inst_list_t *func_list,int *if_count,var_generated_t *var_list);

int generate_func(node_t *node, inst_list_t *func_list,int *if_count,var_generated_t *var_list);

int generate_call_func(node_t *node, inst_list_t *main_body_list);

int generate_call_func_write(node_t *node, inst_list_t *main_body_list);

int generate_func_asign(node_t *node, inst_list_t *main_body_list,var_generated_t *var_list);


int ev_expression(node_t *node, inst_list_t *main_body_list);

int gen_assig_expression(node_t *node, inst_list_t *main_body_list,var_generated_t *var_list);

int generate_if(node_t *node, inst_list_t *main_body_list,int *if_count,inst_list_t *func_list,var_generated_t *var_list);

int generate_while(node_t *node, inst_list_t *main_body_list, inst_list_t *func_list,var_generated_t *var_list,int *if_count);

int generate_return(node_t *node, inst_list_t *main_body_list);