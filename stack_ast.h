#pragma once 
#include "abstact_syntax_tree.h"

typedef struct stackASTNode stackASTNode_t;

struct stackASTNode
{
    ast_t* LastCommand;
    stackASTNode_t* NextNode;
};

typedef struct stackAST
{
    stackASTNode_t *top;
    int size;
}stackAST_t;


stackAST_t initStackAST();
stackASTNode_t* topStackAST(stackAST_t *stack);
int pushStackAST(stackAST_t *stack, ast_t* LastCommand);
void popStackAST(stackAST_t *stack);
void disposeStackAST(stackAST_t *stack);

void addNextCommandToTop(stackAST_t* stack, ast_t* NextCommand);