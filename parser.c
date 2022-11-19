#include "parser.h"
#include "scanner.h"
#include "expressions.h"

token_t scanner_result;
bool scanner_result_is_processed = true;

//Forward declaration

//Rules
int rule_programm(); //PROGRAM => TOKEN_START_TAG TOKEN_PROLOG COMMAND_OR_DECLARE_FUNCTION TOKEN_END_TAG
int command_or_declare_function(); // COMMAND_OR_DECLARE_FUNCTION
int delcare_function(); //rule num (DECLARE_FUNCTION)
int command(); //COMMAND
int command_variable(); //Variable
int command_if();
int command_while();
int command_call_function();
int command_return();
int call_function_or_expresion(); //CALL_FUNCTION_OR_EXPRESION
int call_function();
int parametrs();
int term();
int term_without_epsilon();
int term_int();
int term_float();
int term_string();
int term_var_id();
int term_null();
int term_epsilon();
int term_next();
int parametrs();
int parametrs_without_epsilon();
int parametrs_next();

int data_type();
int func_type();

//Expresion
//int expresion();



int parse()
{
    //Prepare 

    //Rule PROGRAM
    if (rule_programm() == -1)
    {
        printf("fail\n");
        exit(-1);

    }
    
    return 1;
}

void get_unprocessed_token()
{
    if(scanner_result_is_processed)
    {
        scanner_result = get_token(0);
        scanner_result_is_processed = false;
    } 
}

//PROGRAM => TOKEN_PROLOG COMMAND_OR_DECLARE_FUNCTION TOKEN_END_TAG
int rule_programm()
{
    printf("rule_program\n");
    //Add to Parser Tree


    //TOKEN_PROLOG
    get_unprocessed_token();

    if(scanner_result.type != TOKEN_PROLOG)
    {
        //Fail
        return -1;
    }
    scanner_result_is_processed = true;

    //call Rule COMMAND_OR_DECLARE_FUNCTION !!!
    if(command_or_declare_function() != SUCCESS)
    {
        return -1;
    }

    //TOKEN_END_TAG
    get_unprocessed_token();

    if(scanner_result.type != TOKEN_END_TAG)
    {
        //Fail
        return -1;
    }
    scanner_result_is_processed = true;

    return 1;
}

int command_or_declare_function()
{
    printf("comman_or_declare\n");
    //check if it is declare function
    int result = delcare_function();

    if (result == -1)
    {
        //Error !!
        exit(-1);
    }

    if (result == 1)
        return command_or_declare_function();

    if (result == 0)
    {
        return command();
    }

    return SUCCESS;
}


int delcare_function()
{
    printf("declare_func\n");
    //Keywor func
    get_unprocessed_token();

    if(scanner_result.type != TOKEN_KEYWORD)
    {
        return FAIL_IN_BEGIN;
    }

    if(scanner_result.keyword != KEYWORD_FUNCTION)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;


    //create new info in TS

    //ID_OF_FUNC
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_FUNC_ID)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    //add name of func in TS
    
    //(
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_L_PAR)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    //PARAMETERS
    if(parametrs() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }


    //)
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_R_PAR)
    {
        return -1;
    }

    scanner_result_is_processed = true;


    //TOKEN_COLON
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_COLON)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    //FUNC_TYPE
    if(func_type() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //TOKEN_L_BRAC 
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_L_BRAC)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    //COMMAND
    if(command() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //TOKEN_R_BRAC
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_R_BRAC)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    return 1;
}

int command()
{
    printf("command\n");
    int result;
    //Variable
    result = command_variable();
    if (result == 1)
    {
        return command();
    }

    if (result == -1)
    {
        //!!! Error
        exit(-1);
    }

    //If
    result = command_if();
    if (result == 1)
    {
        return command();
    }

    if (result == -1)
    {
        //!!! Error
        exit(-1);
    }


    //While
    result = command_while();
    if (result == 1)
    {
        return command();
    }

    if (result == -1)
    {
        //!!! Error
        exit(-1);
    }

    //Call function
    result = command_call_function();
    if (result == 1)
    {
        return command();
    }

    if (result == -1)
    {
        //!!! Error
        exit(-1);
    }

    //return
    result = command_return();
    if (result == 1)
    {
        return command();
    }

    if (result == -1)
    {
        //!!! Error
        exit(-1);
    }

    //epsilon
    return SUCCESS;
}
//TOKEN_VAR_ID TOKEN_EQUAL CALL_FUNCTION_OR_EXPRESION TOKEN_SEMICOLON COMMAND
int command_variable()
{
    printf("variable\n");
    //TOKEN_VAR_ID
    get_unprocessed_token();
    if (scanner_result.type != TOKEN_VAR_ID)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    //TOKEN_EQUAL
    get_unprocessed_token();
    if (scanner_result.type != TOKEN_EQUAL)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    //CALL_FUNCTION_OR_EXPRESION
    if (call_function_or_expresion() != 1) //???
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;
    //TOKEN_SEMICOLON
    get_unprocessed_token();
    if (scanner_result.type != TOKEN_SEMICOLON)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    return SUCCESS;
}

//KEYWORD_IF TOKEN_L_PAR EXPRESSION TOKEN_R_PAR TOKEN_L_BRAC COMMAND TOKEN_R_BRAC KEYWORD_ELSE TOKEN_L_BRAC COMMAND TOKEN_R_BRAC COMMAND
int command_if()
{
    printf("if\n");
    //KEYWORD_IF
    get_unprocessed_token();
    if (scanner_result.type != TOKEN_KEYWORD)
    {
        return FAIL_IN_BEGIN;
    }

    if(scanner_result.keyword != KEYWORD_IF)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    //TOKEN_L_PAR
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_PAR)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;
    
    //EXPRESSION
    if (expresion(scanner_result) != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //TOKEN_L_BRAC
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    //COMMAND 
    if(command() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }


    //TOKEN_R_BRAC
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_R_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    //KEYWORD_ELSE 
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_KEYWORD)
    {
        return FIAL_IN_MIDDLE;
    }
    if(scanner_result.keyword != KEYWORD_ELSE)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;
    
    //TOKEN_L_BRAC 
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    //COMMAND 
    if(command() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //TOKEN_R_BRAC
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_R_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    return SUCCESS;
}

//KEYWORD_WHILE TOKEN_L_PAR EXPRESSION TOKEN_R_PAR TOKEN_L_BRAC COMMAND TOKEN_R_BRAC COMMAND
int command_while()
{
    printf("while\n");
    // KEYWORD_WHILE
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_KEYWORD)
    {
        return FAIL_IN_BEGIN;
    }
    if(scanner_result.keyword != KEYWORD_WHILE)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true; 

    // TOKEN_L_PAR
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_PAR)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true; 
    //  EXPRESSION 
    if (expresion(scanner_result) != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //  TOKEN_L_BRAC
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true; 

    //  COMMAND 
    if(command() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }


    //  TOKEN_R_BRAC
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_R_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true; 

    return SUCCESS;
}
// CALL_FUNCTION TOKEN_SEMICOLON
int command_call_function()
{
    printf("comand_call_func\n");
    // CALL_FUNCTION 
    int result = call_function();
    
    if (result != SUCCESS)
    {
        return result;
    }
    
    // TOKEN_SEMICOLON
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_SEMICOLON)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    return SUCCESS;
}

// KEYWORD_RETURN EXPRESSION TOKEN_SEMICOLON COMMAND
int command_return()
{
    printf("retrun\n");
    // KEYWORD_RETURN
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_KEYWORD)
    {
        return FAIL_IN_BEGIN;
    }
    if(scanner_result.keyword != KEYWORD_RETURN)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true; 

    //  EXPRESSION
    if (expresion(scanner_result) != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //  TOKEN_SEMICOLON
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_SEMICOLON)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true; 

    return SUCCESS;
}

int call_function_or_expresion()
{
    printf("call_or_exp\n");
    if(call_function() != SUCCESS)
    {
        return expresion(scanner_result);
    }
    return SUCCESS;
}

// TOKEN_FUNC_ID TOKEN_L_PAR TERM TOKEN_R_PAR 
int call_function()
{
    printf("call\n");
    // TOKEN_FUNC_ID
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_FUNC_ID)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    // TOKEN_L_PAR 
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_PAR)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    // TERM
    if(term() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //  TOKEN_R_PAR 
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_R_PAR)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    return SUCCESS;
}

int term()
{
    printf("term\n");
    int result;
    result = term_without_epsilon();
    if (result == FAIL_IN_BEGIN)
    {
        return SUCCESS;
    }

    return FIAL_IN_MIDDLE;
}

int term_without_epsilon()
{
    printf("term_withput_eps\n");
    int result;

    result = term_int();
    if (result == SUCCESS)
    {
        return term_next();
    }

    result = term_float();
    if(result == SUCCESS)
    {
        return term_next();
    }
     
    result = term_var_id();
    if(result == SUCCESS)
    {
        return term_next();
    }

    result = term_string();
    if(result == SUCCESS)
    {
        return term_next();
    }

    result = term_null();
    if(result == SUCCESS)
    {
        return term_next();
    }

    return FAIL_IN_BEGIN;
}

int term_next()
{
    printf("term_next\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_COLON)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    
    if (term_without_epsilon() == FAIL_IN_BEGIN)
    {
        return FIAL_IN_MIDDLE;
    }

    return SUCCESS;

}

int term_int()
{
    printf("term_int\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_INT)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    return SUCCESS;
}

int term_float()
{
    printf("term_float\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_FLOAT)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    return SUCCESS;
}

int term_string()
{
    printf("term_string\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_STRING)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    return SUCCESS;
}

int term_var_id()
{
    printf("term_var_id\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_VAR_ID)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    return SUCCESS;

}

int term_null()
{
    printf("term_null\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_KEYWORD)
    {
        return FAIL_IN_BEGIN;
    }
    if(scanner_result.keyword != KEYWORD_NULL)
    {
        return FAIL_IN_BEGIN;
    }

    scanner_result_is_processed = true;

    return SUCCESS;
}

int parametrs()
{
    printf("parametrs\n");
    if (parametrs_without_epsilon() == FIAL_IN_MIDDLE)
    {
        return FIAL_IN_MIDDLE;
    }
    return SUCCESS;
}

int parametrs_without_epsilon()
{
    printf("parametrs_without_eps\n");
    //DATA_TYPE 
    if(data_type() != SUCCESS)
    {
        return FAIL_IN_BEGIN;
    }

    get_unprocessed_token();
    if(scanner_result.type != TOKEN_VAR_ID)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    int result = parametrs_next();
    if(result == FIAL_IN_MIDDLE)
    {
        return FIAL_IN_MIDDLE;
    }
    
    return SUCCESS;

}

int parametrs_next()
{
    printf("parametrs_next\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_COMMA)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    if(parametrs_without_epsilon() != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    return SUCCESS;
}

int data_type()
{
    printf("data_type\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_KEYWORD)
    {
        return FAIL_IN_BEGIN;
    }

    switch (scanner_result.keyword)
    {
        case KEYWORD_INT:
        case KEYWORD_FLOAT:
        case KEYWORD_STRING:
            scanner_result_is_processed = true;
            return SUCCESS;
            break;
        default:
            return FAIL_IN_BEGIN;
    }
    return FAIL_IN_BEGIN;
}


int func_type()
{
    printf("func_type\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_KEYWORD)
    {
        return FAIL_IN_BEGIN;
    }

    if(scanner_result.keyword != KEYWORD_VOID)
    {
        return data_type();       
    }
    scanner_result_is_processed = true;
    
    return SUCCESS;
}

/*
int expresion()
{
    printf("expresion\n");
    return SUCCESS;
}
*/