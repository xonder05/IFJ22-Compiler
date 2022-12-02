#pragma once 
#include "stack.h"
#include "symtable.h"
#include "abstact_syntax_tree.h"

ast_t* expresion(token_t scanner_result, symTable_t* table, int* result_err);

