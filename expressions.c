/*************************************************
* 
*   file name: expressions.h
*   VUT FIT Project Compiler for language IFJ22
*   Author: Daniel Onderka xonder05
*   Description: Module for dealing with expressions
*
*************************************************/

#include "expressions.h"
#include "error.h"


// converting inputChars used in precedenc parser to operator used in ast_t
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

// converting token structure to inputChars, the intention was to simplify things
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
        
        // when converting acctual values leaf node of ast_t must be created
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
                call_error(SEMANTIC_VAR_ERROR);
                input.type = NOT_ALLOWED_CHAR;
            }
            else
            {
                input.type = I;
                input.data = createExpSubtree(symbol, NULL, NULL, NULL, NULL);
            }
            break;
        
        case TOKEN_KEYWORD:
            if (token.keyword == KEYWORD_NULL)
            {
                input.type = I;
                input.data = createExpSubtree(NULL, NULL, NULL, NULL, NULL);
                break;
            }
            else
            {            
                call_error(SYNTAX_ERROR);
                input.type = NOT_ALLOWED_CHAR;
                break;
            }
        case TOKEN_BLANK0:
        case TOKEN_BLANK1:
        case TOKEN_BLANK2:
        case TOKEN_BLANK3:
            call_error(LEX_ERROR);
            break;
        default:
            call_error(SYNTAX_ERROR);
            input.type = NOT_ALLOWED_CHAR;
        }
    return input;
}

// in case token passed to expression is keyword which can be followed by expression
stackItem_t findFirstExpressionChar(token_t token, symTable_t* table)
{
    if (token.type == TOKEN_KEYWORD && token.keyword != KEYWORD_NULL)
    {
        token = get_token();
    }
    stackItem_t item = convertrToken(token, table);
    return item;
}

// standart way to get new input
stackItem_t getNewInput(symTable_t* table)
{
    token_t token = get_token();
    stackItem_t input = convertrToken(token, table);
    return input;
}

// finds the nonterminal closest to top of stack
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

// called when shifting to stack, inserts handle above nonterminal closest to top of stack
int insertHandle(stack_t *stack)
{
    stackItem_t handle = closestToTopNonTerminal(stack);
    stackItem_t top = topStack(stack, 0);

    if (handle.type == EMPTY || top.type == EMPTY)
    {
        call_error(OTHERS_ERROR);
    }

    stackItem_t *ptr = malloc(sizeof(stackItem_t));
    if (ptr == NULL) 
    {
        call_error(OTHERS_ERROR);
    }
    
    int i = 0;
    while (top.type != handle.type)
    {
        popStack(stack);
        i++;
        ptr = realloc(ptr, sizeof(stackItem_t)*i);
        if (ptr == NULL)
        {
            return 1;
        }

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
    return 0;
}

// operation from precedenc table
// < shift input to stack
int shiftToStack(stack_t *stack, stackItem_t input)
{
    if (insertHandle(stack) == true)
    {
        return 1;
    }
    if (pushStack(stack, input) == NULL)
    {
        return 1;
    }
    return 0;
}

// operation from precedenc table
// reduce sequence on stack into one expression
int reduce(stack_t *stack)
{
    if (stack == NULL)
    {
        call_error(OTHERS_ERROR);
    }

    stackItem_t top = topStack(stack, 0);
    if (top.type == EMPTY)
    {
        return 1;
    }

// three ways to reduce items on stack  
    if (top.type == I) //STACK:  I -> H
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
    else if (top.type == RPAR) //STACK: ( -> E -> ) -> H
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
    else if (top.type == EXPRESSION) //STACK: E -> OP -> E -> H
    {
        InputChars i = topStack(stack, 1).type;
        if ( (i == PLUS || i == MINUS || i == MULTIPLY || i == DIVIDE || i == DOT 
            || i == EQUAL || i == NOTEQUAL || i == GREATER || i == GREATER_EQUAL || i == LESSER || i == LESSER_EQUAL)
            && topStack(stack, 2).type == EXPRESSION && topStack(stack,3).type == HANDLE)
        {
            stackItem_t left = topStack(stack, 0);
            stackItem_t right = topStack(stack, 2);
            stackItem_t op = topStack(stack, 1);

            if (convertInputCharsToOperator(op.type) == -1) 
            { 
                call_error(SEMANTIC_COMPABILITY_ERROR);
            }

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
    switch (input.type)
    {
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: 
     case I: case LPAR: case RPAR:
        *nextInput = true;
        return shiftToStack(stack, input);

    default:
        return 1;
    }
}

int plus(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case MULTIPLY: case DIVIDE: case LPAR: case I:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int minus(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);   
         
    case MULTIPLY: case DIVIDE: case LPAR: case I:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int multipy(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: 
    case RPAR: case MULTIPLY: case DIVIDE: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case LPAR: case I:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int divide(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: 
    case RPAR: case MULTIPLY: case DIVIDE: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case LPAR: case I:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int dot(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
    case PLUS: case MINUS: case DOT: case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case MULTIPLY: case DIVIDE: case LPAR: case I:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int equal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
    case EQUAL: case NOTEQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL:  case LPAR: case I:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int notequal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {    
    case EQUAL: case NOTEQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case LPAR: case I:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int greater(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int greater_equal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);

    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int lesser(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int lesser_equal(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL: case RPAR: case END_OF_EXPRESSION:
        *nextInput = false;
        return reduce(stack);
    
    case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        *nextInput = true;
        return shiftToStack(stack, input);
    
    default:
        return 1;
    }
}
int lpar(stack_t *stack, stackItem_t input, bool *nextInput)
{
    switch (input.type)
    {
     case EQUAL: case NOTEQUAL: case GREATER: case GREATER_EQUAL: case LESSER: case LESSER_EQUAL:
     case PLUS: case MINUS: case MULTIPLY: case DIVIDE: case DOT:  case I: case LPAR:
        *nextInput = true;
        return shiftToStack(stack, input);

    case RPAR:
        *nextInput = true;
        input.data = NULL;
        input.type = RPAR;
        if (pushStack(stack, input) == NULL)
        {
            return 1;
        }
        else
        {
            return 0;
        }

    default:
        return 1;
    }
}
int rpar(stack_t *stack, stackItem_t input, bool *nextInput)
{
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
    if (table == NULL) 
    {
        call_error(OTHERS_ERROR);
    }
    
    //init
    bool nextInput = false;
    bool fail = false;
    stack_t *stack = NULL;
    stackItem_t input;
    stackItem_t stacktop;


    stack = initStack(stack);
    if (stack == NULL) 
    {
        return NULL;
    }
    input.data = NULL;
    input.type = STARTEND;
    stack = pushStack(stack, input);
    if (stack == NULL) 
    { 
        fail = true; 
    }

    input = findFirstExpressionChar(scanner_result, table);
    if (input.type == NOT_ALLOWED_CHAR) 
    { 
        fail = true; 
    }
    
    stacktop = closestToTopNonTerminal(stack);
    if (stacktop.type == EMPTY) 
    {
        fail = true;
    }


    //main cycle
    // reprezents picking a line in precedence table (based on neterminal closest to top of stack)
    do{
        if (fail == true) 
        {
            break;
        }

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
            fail = true;
            call_error(SEMANTIC_COMPABILITY_ERROR);
        }
        if (fail == true)
        {
            break; 
        }

        // getting new input and checking it's validity
        if (nextInput == true)
        {
            nextInput = false;
            input = getNewInput(table);
            if (input.type == NOT_ALLOWED_CHAR) 
            {
                fail = true; 
                break;
            }
        }

        // updating what is on top of stack
        stacktop = closestToTopNonTerminal(stack);
        if (stacktop.type == EMPTY) 
        {
            call_error(OTHERS_ERROR);
        }

    } while (input.type != END_OF_EXPRESSION || topStack(stack, 0).type != EXPRESSION || topStack(stack, 1).type != STARTEND);
    //if you found terminal which can follow after expression and stack has only one expression on it => end the cycle


    if (fail == true) // if the cycle ended because of error
    {
        if (stack->items == 1 && stack->top->type == STARTEND) // if the expression is empty
        {
            diposeExpSubtree(stack->top->data);
            disposeStack(stack);
            *result_err = 0;
            return NULL;
        }
        // other error
        diposeExpSubtree(stack->top->data);
        disposeStack(stack);
        *result_err = -1;
        return NULL;
    }
    else // if the cycle ended in natural way
    {
        // creating resulting syntax tree
        stackItem_t item = topStack(stack, 0);
        ast_t* result = NULL;
        
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
            call_error(OTHERS_ERROR);
            break;
        }

        disposeStack(stack);
        *result_err = 1;
        return result;
    }
}




