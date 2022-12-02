#include "stack_ast.h"

stackAST_t initStackAST()
{
    stackAST_t stack;
    stack.top = NULL;
    stack.size = 0;
    return stack;
}

stackASTNode_t* topStackAST(stackAST_t *stack)
{
    return stack->top;
}

int pushStackAST(stackAST_t *stack, ast_t* LastCommand)
{
    stackASTNode_t* NewNode = malloc(sizeof(stackASTNode_t));
    if (NewNode == NULL)
    {
        return -1;
    }

    NewNode->LastCommand = LastCommand;
    NewNode->NextNode = stack->top;

    stack->top = NewNode;
    stack->size++;
    return 1;
}

void popStackAST(stackAST_t *stack)
{
    if (stack->size > 0)
    {
        stack->size--;
        stackASTNode_t *target = stack->top;
        stack->top = target->NextNode;
        free(target);
    }
}

void disposeStackAST(stackAST_t *stack)
{
    while (topStackAST(stack))
    {
        popStackAST(stack);
    }
}


void addNextCommandToTop(stackAST_t* stack, ast_t* NextCommand)
{
    stack->top->LastCommand->nextcommand = NextCommand;
    stack->top->LastCommand = NextCommand;
}