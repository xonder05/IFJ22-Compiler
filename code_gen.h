#pragma once
#include "scanner.h"
#include "abstact_syntax_tree.h"

// bool generate_operation(Token_type operator_f,char *first_operand,char *second_operand)

//parametr strom
int generate();

//parametry strom, main_list,func_list
int gen_from_ass();

int generate_func(node_t *node, inst_list_t *func_list);
