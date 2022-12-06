/*************************************************
* 
*   file name: main.c
*   VUT FIT Project Compiler for language IFJ22
*   Authors:
*       Daniel Onderka xonder05
*       Ondřej Bahounek xbahou00
*       Aleksandr Kasinova xkasia01
*       Tomáš Prokop xproko49
*   Description: Main file of compiler
*
*************************************************/

#include <stdlib.h>
#include "scanner.h"
#include "code_gen.h"
#include "abstact_syntax_tree.h"
#include "symtable.h"
#include "parser.h"

int main()
{
    
    ast_t* AST = createRootNode(false);
    symTable_t* Table = initSymTable();

    
    parse(Table, AST);

    generate(AST);  

    disposeTree(AST);
    freeSymTable(Table);

    return 0;
}