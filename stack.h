/*************************************************
* 
*   file name: stack.h
*   VUT FIT Project Compiler for language IFJ22
*   Author: Daniel Onderka xonder05
*   Description: Implementation of stack used for storing expressions
*
*************************************************/

#pragma once 
#include "scanner.h"
#include "abstact_syntax_tree.h"

typedef enum
{
    EMPTY, STARTEND, HANDLE, EXPRESSION, I,
    PLUS, MINUS, MULTIPLY, DIVIDE, DOT,
    EQUAL, NOTEQUAL, GREATER, GREATER_EQUAL, LESSER, LESSER_EQUAL, 
    LPAR, RPAR,
    END_OF_EXPRESSION, NOT_ALLOWED_CHAR

} InputChars;

typedef struct stack
{
    struct stackItem *top; //top of the stack
    int items; //amount of items in stack
}stack_t;

typedef struct stackItem
{
    InputChars type;
    exp_subtree_t *data;
    struct stackItem *next;
} stackItem_t;




//success - new stack
//fail - null
stack_t *initStack(stack_t *stack);

//success - found item 
//fail - item.type = empty
stackItem_t topStack(stack_t *stack, int fromTop); // fromTop == 0 returns top of stack

//success - stack
//fail - null
stack_t* pushStack(stack_t *stack, stackItem_t item);
stack_t *popStack(stack_t *stack);
void disposeStack(stack_t *stack);
void printStack(stack_t *stack);
int sizeStack(stack_t* stack);