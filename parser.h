/*************************************************
* 
*   file name: parser.h
*   VUT FIT Project Compiler for language IFJ22
*   Author: Aleksandr Kasinova xkasia01
*   Description: Top-down syntactic analysis
*
*************************************************/

#pragma once 

#include "abstact_syntax_tree.h"


int parse(symTable_t* Table, ast_t* AST);