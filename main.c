#include "parser.h"
#include "abstact_syntax_tree.h"
#include "symtable.h"
#include "code_gen.h"
#include <stdio.h>

int main ()
{

    ast_t* AST = createRootNode(false);
    symTable_t* Table = initSymTable();
    parse(Table, AST);


    //printf("\n\nstart\n\n");
  // printTree(AST);

    //printf("\n\n Gen codu \n\n");

    generate(AST);

    return 0;
}