#include "stack.h"


stack_t* initStack(stack_t *stack)
{
    stack = malloc(sizeof(struct stack));
    if (stack == NULL) { fprintf(stderr, "malloc error\n"); return NULL; }
    stack->top = NULL;
    stack->items = 0;
    return stack;
}

stackItem_t topStack(stack_t *stack, int fromTop)
{
    if (stack->items > fromTop)
    {
        stackItem_t item = *stack->top;
        for (int i = 0; i < fromTop; i++)
        {
            item = *item.next;
        }            
        return item;
    }
    stackItem_t item;
    item.data = NULL;
    item.next = NULL;
    item.type = EMPTY;
    return item;
}

stack_t* pushStack(stack_t *stack, stackItem_t inputItem)
{
    stackItem_t *item = malloc(sizeof(struct stackItem));
    if (item == NULL) { fprintf(stderr, "malloc error\n"); return NULL; }

    item->next = stack->top;
    item->type = inputItem.type;
    item->data = inputItem.data;
    stack->items++;
    stack->top = item;
    return stack;
}


stack_t* popStack(stack_t *stack)
{
    if (stack->items > 0)
    {
        stack->items--;
        stackItem_t *item = stack->top;
        stack->top = item->next;
        free(item);
    }

    return stack;
}

void disposeStack(stack_t *stack)
{
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