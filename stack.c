#include "stack.h"
#include "error.h"

stack_t* initStack(stack_t *stack)
{
    stack = malloc(sizeof(struct stack));
    if (stack == NULL) 
    { 
        call_error(OTHERS_ERROR); 
    }

    stack->top = NULL;
    stack->items = 0;
    return stack;
}

stackItem_t topStack(stack_t *stack, int fromTop)
{
    stackItem_t item;
    if (stack != NULL)
    {
        if (stack->items > fromTop)
        {
            item = *stack->top;
            for (int i = 0; i < fromTop; i++)
            {
                item = *item.next;
            }            
            return item;
        }
    }
    item.data = NULL;
    item.next = NULL;
    item.type = EMPTY;
    return item;
}

stack_t* pushStack(stack_t *stack, stackItem_t inputItem)
{
    if (stack == NULL) 
    {
        return NULL;
    }

    stackItem_t *item = malloc(sizeof(struct stackItem));
    if (item == NULL) 
    { 
        call_error(OTHERS_ERROR);
    }

    *item = inputItem;
    item->next = stack->top;

    stack->items++;
    stack->top = item;

    return stack;
}

stack_t* popStack(stack_t *stack)
{
    if (stack == NULL) 
    {
        return NULL;
    }

    if (stack->items > 0)
    {
        stack->items--;
        stackItem_t *item = stack->top;
        stack->top = item->next;
        free(item);
        return stack;
    }
    else
    {
        return NULL;
    }
}

void disposeStack(stack_t *stack)
{
    if (stack == NULL) 
    {
        return ;
    }

    while (topStack(stack, 0).type != EMPTY)
    {
        popStack(stack);
    }
    free(stack);
}


void printStack(stack_t *stack)
{
    printf("Stack:\nTOP -> ");
    int i = 0;
    stackItem_t top;
    while (true)
    {
        top = topStack(stack, i);
        if (top.type != EMPTY)
        {
            printf("%d -> ",top.type);
        }
        else   
        {
            printf("END\n");
            break;
        }
        i++;
    }
}

int sizeStack(stack_t* stack)
{
    int i = 0;
    stackItem_t top;
    while (true)
    {
        top = topStack(stack, i);
        if (top.type == EMPTY)
        {
            break;
        }
        i++;
    }

    return i;
}