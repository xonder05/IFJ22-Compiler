/*************************************************
* 
*   file name: expressions.h
*   VUT FIT Project Compiler for language IFJ22
*   Author: Daniel Onderka xonder05
*   Description: Module for dealing with expressions
*
*************************************************/

#pragma once 
#include "stack.h"
#include "symtable.h"
#include "abstact_syntax_tree.h"

ast_t* expresion(token_t scanner_result, symTable_t* table, int* result_err);
