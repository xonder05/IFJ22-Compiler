#include "expressions.h"
#include "stack.h"


InputChars convertrToken(token_t token)
{
    //printf("token %d\n", token.type);
    InputChars input;
    switch (token.type){
        case TOKEN_PLUS:
            input = PLUS;
            break;

        case TOKEN_MINUS:
            input = MINUS;
            break;

        case TOKEN_MULTIPLY:
            input = MULTIPLY;
            break;

        case TOKEN_DIVIDE:
            input = DIVIDE;
            break;

        case TOKEN_DOT:
            input = DOT;
            break;

        case TOKEN_BIGGER:
            input = GREATER;
            break;

        case TOKEN_BIGGER_EQ:
            input = GREATER_EQUAL;
            break;

        case TOKEN_SMALLER:
            input = LESSER;
            break;

        case TOKEN_SMALL_EQ:
            input = LESSER_EQUAL;
            break;
        
        case TOKEN_EQUAL2:
            input = EQUAL;
            break;
        
        case TOKEN_NOT_EQUAL:
            input = NOTEQUAL;
            break;
        
        case TOKEN_L_PAR:
            input = LPAR;
            break;

        case TOKEN_R_PAR:
            input = RPAR;
            break;

        case TOKEN_L_BRAC:
            input = END_OF_EXPRESSION;
            ungetc('{', stdin);
            break;

        case TOKEN_SEMICOLON:
            input = END_OF_EXPRESSION;
            ungetc(';', stdin);
            break;
        
        case TOKEN_INT: case TOKEN_FLOAT: case TOKEN_STRING: case TOKEN_VAR_ID:
            input = I;
            break;
        
        case TOKEN_KEYWORD:
            if (token.keyword == KEYWORD_NULL)
            {
                input = I;
                break;
            }
            else
            {            
                fprintf(stderr, "token not allowed in expression\n");
                input = NOT_ALLOWED_CHAR;
                break;
            }
        default:
            fprintf(stderr, "token not allowed in expression\n");
            input = NOT_ALLOWED_CHAR;
        }
    return input;
}

InputChars findFirstExpressionChar(token_t token)
{
    if (token.type == TOKEN_KEYWORD && token.keyword != KEYWORD_NULL)
    {
        token = get_token(0);
    }
    return convertrToken(token);
}

InputChars getNewInput()
{
    token_t token = get_token(0);
    InputChars input = convertrToken(token);
    return input;
}

InputChars closestToTopNonTerminal(stack_t *stack)
{
    int i = 0;
    InputChars stacktop = topStack(stack, i);
    while(stacktop == EXPRESSION || stacktop == HANDLE)
    {
        stacktop = topStack(stack, i);
        i++;
    }
    return stacktop;
}

void insertHandle(stack_t *stack)
{
    InputChars *ptr = malloc(sizeof(InputChars)); 
    InputChars handle = closestToTopNonTerminal(stack);
    InputChars top = topStack(stack, 0);
    int i = 0;
    while (top != handle)
    {
        popStack(stack);
        i++;
        ptr = realloc(ptr, sizeof(InputChars)*i);
        ptr[i-1] = top;
        top = topStack(stack, 0);
    }

    pushStack(stack, HANDLE);

    while (i > 0)
    {
        pushStack(stack, ptr[i-1]);
        i--;
    }
    
    free(ptr);
}

void shiftToStack(stack_t *stack, InputChars input)
{
    //printf("shifttostack\n");
    insertHandle(stack);
    pushStack(stack, input);
}

int reduce(stack_t *stack)
{
    //printf("reduce\n");
    int top = topStack(stack, 0);
    if (top == I)
    {
        if (topStack(stack, 1) == HANDLE )
        {
            popStack(stack);
            popStack(stack);
            pushStack(stack, EXPRESSION);
            return 0;
        }
        else
        {
            return 1;
        } 
    }
    else if (top == RPAR)
    {
        if (topStack(stack, 1) == EXPRESSION && topStack(stack, 2) == LPAR && topStack(stack,3) == HANDLE)
        {
            popStack(stack);
            popStack(stack);
            popStack(stack);
            popStack(stack);
            pushStack(stack, EXPRESSION);
            return 0;
        }
        else
        {
            return 1;
        }

    }
    else if (top == EXPRESSION)
    {
        if ( (topStack(stack, 1) == PLUS || topStack(stack, 1) == MINUS || topStack(stack, 1) == MULTIPLY || topStack(stack, 1) == DIVIDE || topStack(stack, 1) == DOT 
        || topStack(stack, 1) == EQUAL || topStack(stack, 1) == NOTEQUAL || topStack(stack, 1) == GREATER || topStack(stack, 1) == GREATER_EQUAL 
        || topStack(stack, 1) == LESSER || topStack(stack, 1) == LESSER_EQUAL)
        && topStack(stack, 2) == EXPRESSION && topStack(stack,3) == HANDLE)
        {
            popStack(stack);
            popStack(stack);
            popStack(stack);
            popStack(stack);
            pushStack(stack, EXPRESSION);
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
    return 1;
}


int startend(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("startend\n");
    switch (input)
    {
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: 
     case I: case LPAR: case RPAR:
        *nextInput = true;
        shiftToStack(stack, input);
        return 0;

    default:
        return 1;
    }
}

int plus(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("plus\n");
    switch (input)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case MULTIPLY: case DIVIDE: case LPAR: case I:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}

int minus(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("minus\n");
    switch (input)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);   
         
    case MULTIPLY: case DIVIDE: case LPAR: case I:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
    return 0;
}
int multipy(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("multiply\n");
    switch (input)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: 
    case RPAR: case MULTIPLY: case DIVIDE: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case LPAR: case I:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int divide(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("divide\n");
    switch (input)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: 
    case RPAR: case MULTIPLY: case DIVIDE: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case LPAR: case I:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int dot(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("dot\n");
    switch (input)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case MULTIPLY: case DIVIDE: case LPAR: case I:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int equal(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("equal\n");
    switch (input)
    {
    case EQUAL: case NOTEQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL:  case LPAR: case I:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int notequal(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("notequal\n");
    switch (input)
    {    
    case EQUAL: case NOTEQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case LPAR: case I:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int greater(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("greater\n");
    switch (input)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int greater_equal(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("greaterequal\n");
    switch (input)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int lesser(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("lesser\n");
    switch (input)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int lesser_equal(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("lesserequal\n");
    switch (input)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;
    
    default:
        return 1;
    }
}
int lpar(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("lpar\n");
    switch (input)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL:
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;

    case RPAR:
        pushStack(stack, RPAR);
        *nextInput = true;
        return 0;

    default:
        return 1;
    }
}
int rpar(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("rpar\n");
    switch (input)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR:
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    default:
        return 1;
    }
}

int i(stack_t *stack, InputChars input, bool *nextInput)
{
    //printf("i\n");
    switch (input)
    {
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT: case RPAR:
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    default:
        return 1;
    }
}

int expresion(token_t scanner_result)
{
    //printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
    //printf("scanner result %d\n", scanner_result.type);
    
    stack_t *stack = NULL;
    stack = initStack(stack);
    stack = pushStack(stack, STARTEND);

    InputChars input = findFirstExpressionChar(scanner_result);
    InputChars stacktop = closestToTopNonTerminal(stack);
    bool nextInput = false;
    bool fail = false;

    do{
        //printf("--------------new round--------------\n");
        //printf("input %d, stacktop %d\n", input, stacktop);
        //printStack(stack);
        switch (stacktop)
        {
        case STARTEND :
            fail = startend(stack, input, &nextInput);
            break;
        case PLUS :
            fail = plus(stack, input, &nextInput);
            break;
        case MINUS :
            fail = minus(stack, input, &nextInput);
            break;
        case MULTIPLY :
            fail = multipy(stack, input, &nextInput);
            break;
        case DIVIDE :
            fail = divide(stack, input, &nextInput);
            break;
        case DOT :
            fail = dot(stack, input, &nextInput);
            break;
        case EQUAL :
            fail = equal(stack, input, &nextInput);
            break;
        case NOTEQUAL :
            fail = notequal(stack, input, &nextInput);
            break;
        case GREATER :
            fail = greater(stack, input, &nextInput);
            break;
        case GREATER_EQUAL :
            fail = greater_equal(stack, input, &nextInput);
            break;
        case LESSER :
            fail = lesser(stack, input, &nextInput);
            break;
        case LESSER_EQUAL :
            fail = lesser_equal(stack, input, &nextInput);
            break;
        case LPAR :
            fail = lpar(stack, input, &nextInput);
            break;
        case RPAR :
            fail = rpar(stack, input, &nextInput);
            break;
        case I :
            fail = i(stack, input, &nextInput);
            break;

        default :
        break;
        }

        if (fail == true)
        {
            break;
        }

        if (nextInput == true)
        {
            nextInput = false;
            input = getNewInput();
        }
        stacktop = closestToTopNonTerminal(stack);

    } while (input != END_OF_EXPRESSION || topStack(stack, 0) != EXPRESSION || topStack(stack, 1) != STARTEND);


    //printStack(stack);
    disposeStack(stack);

    if (fail == true)
    {
        fprintf(stderr, "expression fail\n");
        //printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
        return 0;
    }
    else
    {
        printf("expression success\n");
        //printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
        return 1;
    }

}




