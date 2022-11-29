#pragma once 
#include "scanner.h"

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
    struct stackItem *top;
    int items;

}stack_t;

typedef struct stackItem
{
    InputChars type;
    struct stackItem *next;

} stackItem_t;

stack_t *initStack(stack_t *stack);
InputChars topStack(stack_t *stack, int fromTop); // fromTop == 0 returns top of stack
stack_t *pushStack(stack_t *stack, InputChars token);
stack_t *popStack(stack_t *stack);
void disposeStack(stack_t *stack);
void printStack(stack_t *stack);
int sizeStack(stack_t* stack);
// return values 0 success 1 fail
