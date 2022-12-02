#include "expressions.h"

operator convertInputCharsToOperator(InputChars input)
{
        int output;
        switch (input)
        {
        case PLUS:
            output = Plus;
            break;
        case MINUS:
            output = Minus;
            break;
        case MULTIPLY:
            output = Multiply;
            break;
        case DIVIDE:
            output = Divide;
            break;
        case DOT:
            output = Dot;
            break;
        case EQUAL:
            output = Equal;
            break;
        case NOTEQUAL:
            output = NotEqual;
            break;
        case GREATER:
            output = Greater;
            break;
        case GREATER_EQUAL:
            output = GreaterEqual;
            break;
        case LESSER:
            output = Lesser;
            break;
        case LESSER_EQUAL:
            output = LesserEqual;
            break;
        default:
            output = Error;
        break;
        }
        return output;
}



stackItem_t convertrToken(token_t token, symTable_t* table)
{
    stackItem_t input;
    symbol_t *symbol; 

    switch (token.type){
        case TOKEN_PLUS:
            input.type = PLUS;
            break;

        case TOKEN_MINUS:
            input.type = MINUS;
            break;

        case TOKEN_MULTIPLY:
            input.type = MULTIPLY;
            break;

        case TOKEN_DIVIDE:
            input.type = DIVIDE;
            break;

        case TOKEN_DOT:
            input.type = DOT;
            break;

        case TOKEN_BIGGER:
            input.type = GREATER;
            break;

        case TOKEN_BIGGER_EQ:
            input.type = GREATER_EQUAL;
            break;

        case TOKEN_SMALLER:
            input.type = LESSER;
            break;

        case TOKEN_SMALL_EQ:
            input.type = LESSER_EQUAL;
            break;
        
        case TOKEN_EQUAL2:
            input.type = EQUAL;
            break;
        
        case TOKEN_NOT_EQUAL:
            input.type = NOTEQUAL;
            break;
        
        case TOKEN_L_PAR:
            input.type = LPAR;
            break;

        case TOKEN_R_PAR:
            input.type = RPAR;
            break;

        case TOKEN_L_BRAC:
            input.type = END_OF_EXPRESSION;
            ungetc('{', stdin);
            break;

        case TOKEN_SEMICOLON:
            input.type = END_OF_EXPRESSION;
            ungetc(';', stdin);
            break;
        
        case TOKEN_INT:
            input.type = I;
            input.data = createExpSubtree(NULL, NULL, &token.int_value, NULL, NULL);
            break;

        case TOKEN_FLOAT:
            input.type = I;
            input.data = createExpSubtree(NULL, NULL, NULL, &token.float_value, NULL);
            break; 

        case TOKEN_STRING:
            input.type = I;
            input.data = createExpSubtree(NULL, NULL, NULL, NULL, token.string);
            break;

        case TOKEN_VAR_ID:
            symbol = findSymTableInCurentConxtext(table, token.string);
            if (symbol == NULL)
            {
                fprintf(stderr, "undefined variable\n");
                exit(0);
            }
            input.type = I;
            input.data = createExpSubtree(symbol, NULL, NULL, NULL, NULL);
            break;
        
        case TOKEN_KEYWORD:
            if (token.keyword == KEYWORD_NULL)
            {
                input.type = I;
                input.data = createExpSubtree(NULL, NULL, NULL, NULL, NULL);
                //printf("data type %d\n", input.data->type);
                break;
            }
            else
            {            
                fprintf(stderr, "token not allowed in expression\n");
                input.type = NOT_ALLOWED_CHAR;
                break;
            }
        default:
            //printf("token type %d\n", token.type);
            fprintf(stderr, "token not allowed in expression\n");
            input.type = NOT_ALLOWED_CHAR;
        }
    return input;
}

stackItem_t findFirstExpressionChar(token_t token, symTable_t* table)
{
    if (token.type == TOKEN_KEYWORD && token.keyword != KEYWORD_NULL)
    {
        token = get_token();
    }
    stackItem_t item = convertrToken(token, table);
    return item;
}

stackItem_t getNewInput(symTable_t* table)
{
    token_t token = get_token();
    stackItem_t input = convertrToken(token, table);
    return input;
}

stackItem_t closestToTopNonTerminal(stack_t *stack)
{
    int i = 0;
    stackItem_t stacktop = topStack(stack, i);
    while(stacktop.type == EXPRESSION || stacktop.type == HANDLE)
    {
        stacktop = topStack(stack, i);
        i++;
    }
    return stacktop;
}

void insertHandle(stack_t *stack)
{
    stackItem_t *ptr = malloc(sizeof(stackItem_t)); 
    stackItem_t handle = closestToTopNonTerminal(stack);
    stackItem_t top = topStack(stack, 0);
    int i = 0;
    while (top.type != handle.type)
    {
        popStack(stack);
        i++;
        ptr = realloc(ptr, sizeof(stackItem_t)*i);
        ptr[i-1] = top;
        top = topStack(stack, 0);
    }

    handle.type = HANDLE;
    handle.data = NULL;
    pushStack(stack, handle);

    while (i > 0)
    {
        pushStack(stack, ptr[i-1]);
        i--;
    }
    
    free(ptr);
}

void shiftToStack(stack_t *stack, stackItem_t input)
{
    //printf("shifttostack\n");
    insertHandle(stack);
    pushStack(stack, input);
}

int reduce(stack_t *stack)
{
    //printf("reduce\n");
    stackItem_t top = topStack(stack, 0);
    if (top.type == I)
    {
        if (topStack(stack, 1).type == HANDLE )
        {
            popStack(stack);
            popStack(stack);
            top.type = EXPRESSION;
            pushStack(stack, top);
            return 0;
        }
        else
        {
            return 1;
        } 
    }
    else if (top.type == RPAR)
    {
        if (topStack(stack, 1).type == EXPRESSION && topStack(stack, 2).type == LPAR && topStack(stack,3).type == HANDLE)
        {
            popStack(stack); //pop rpar
            top = topStack(stack, 0); //save expression
            popStack(stack); //pop expression
            popStack(stack); //pop lpar
            popStack(stack); //pop handle
            top.type = EXPRESSION;
            pushStack(stack, top);
            return 0;
        }
        else
        {
            return 1;
        }

    }
    else if (top.type == EXPRESSION)
    {
        if ( (topStack(stack, 1).type == PLUS || topStack(stack, 1).type == MINUS || topStack(stack, 1).type == MULTIPLY 
        || topStack(stack, 1).type == DIVIDE || topStack(stack, 1).type == DOT 
        || topStack(stack, 1).type == EQUAL || topStack(stack, 1).type == NOTEQUAL || topStack(stack, 1).type == GREATER 
        || topStack(stack, 1).type == GREATER_EQUAL || topStack(stack, 1).type == LESSER || topStack(stack, 1).type == LESSER_EQUAL)
        && topStack(stack, 2).type == EXPRESSION && topStack(stack,3).type == HANDLE)
        {
            stackItem_t left = topStack(stack, 0);
            stackItem_t right = topStack(stack, 2);
            stackItem_t op = topStack(stack, 1);
            if (convertInputCharsToOperator(op.type) == -1) { fprintf(stderr, "convert fail\n"); exit(0);}
            ast_t* node = createExpressionNode(convertInputCharsToOperator(op.type), left.data, right.data);
            stackItem_t item;
            item.type = EXPRESSION;
            item.data = createExpSubtree(NULL, node, NULL, NULL, NULL);
            popStack(stack);
            popStack(stack);
            popStack(stack);
            popStack(stack);
            pushStack(stack, item);
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


int startend(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("startend\n");
    switch (input.type)
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

int plus(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("plus\n");
    switch (input.type)
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

int minus(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("minus\n");
    switch (input.type)
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
int multipy(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("multiply\n");
    switch (input.type)
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
int divide(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("divide\n");
    switch (input.type)
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
int dot(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("dot\n");
    switch (input.type)
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
int equal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("equal\n");
    switch (input.type)
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
int notequal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("notequal\n");
    switch (input.type)
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
int greater(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("greater\n");
    switch (input.type)
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
int greater_equal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("greaterequal\n");
    switch (input.type)
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
int lesser(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("lesser\n");
    switch (input.type)
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
int lesser_equal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("lesserequal\n");
    switch (input.type)
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
int lpar(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("lpar\n");
    switch (input.type)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL:
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        shiftToStack(stack, input);
        *nextInput = true;
        return 0;

    case RPAR:
        input.data = NULL;
        input.type = RPAR;
        pushStack(stack, input);
        *nextInput = true;
        return 0;

    default:
        return 1;
    }
}
int rpar(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("rpar\n");
    switch (input.type)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR:
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    default:
        return 1;
    }
}

int i(stack_t *stack, stackItem_t input, bool *nextInput)
{
    //printf("i\n");
    switch (input.type)
    {
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT: case RPAR:
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    default:
        return 1;
    }
}

ast_t* expresion(token_t scanner_result, symTable_t* table, int* result_err)
{
    //printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
    //printf("scanner result %d\n", scanner_result.type);
    
    stack_t *stack = NULL;
    stack = initStack(stack);
    stackItem_t input;
    input.data = NULL;
    input.type = STARTEND;
    stack = pushStack(stack, input);

    input = findFirstExpressionChar(scanner_result, table);
    stackItem_t stacktop = closestToTopNonTerminal(stack);
    bool nextInput = false;
    bool fail = false;

    do{
        //printf("--------------new round--------------\n");
        //printf("input %d, stacktop %d\n", input.type, stacktop.type);
        //printStack(stack);
        switch (stacktop.type)
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
            input = getNewInput(table);
        }
        stacktop = closestToTopNonTerminal(stack);
        

    } while (input.type != END_OF_EXPRESSION || topStack(stack, 0).type != EXPRESSION || topStack(stack, 1).type != STARTEND);


    //printStack(stack);

    stackItem_t item = topStack(stack, 0);
    ast_t* result;
    
    switch (item.data->type)
    {
    case imm:
        result = createExpressionNode(SingleOp, item.data, NULL);
        break;
    case op:
        result = createExpressionNode(SingleOp, item.data, NULL);
        break;
    case nul:
        result = createExpressionNode(SingleOp, item.data, NULL);
        break;
    case exp:
        result = item.data->data.exp;
        break;
    default:
        fprintf(stderr,"error\n");   
        exit(0);
        break;
    }

    //printTree(result);



    disposeStack(stack);

    if (fail == true)
    {
        if (sizeStack(stack) == 1 && stack->top->type == STARTEND)
        {
            
            fprintf(stderr, "expression none\n");
            disposeStack(stack);
            *result_err = 0;
            return NULL;
        }

        fprintf(stderr, "expression fail\n");
        //printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
        *result_err = -1;
        return NULL;
    }
    else
    {
        printf("expression success\n");
        //printf("-----------------------------------------------------------------------------------------------------------------------------------\n");
        *result_err = 1;
        return result;
    }

}




