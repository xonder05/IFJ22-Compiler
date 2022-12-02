#include "parser.h"
#include "scanner.h"
#include "expressions.h"
#include "symtable.h"
#include "error.h"
#include "stack_ast.h"
#include "abstact_syntax_tree.h"

#include "testing_utils.h"

token_t scanner_result;
bool scanner_result_is_processed = true;

enum rule_result
{
    FAIL_IN_BEGIN = 0,
    FIAL_IN_MIDDLE = -1,
    SUCCESS = 1
};

enum table_sintax
{
    LEFT_TABLE = 2,  // <
    RIGHT_TABLE = 1, // >
    EQUAL_TABLE = 0, //=
    ERORR_TABLE = 3  //empty
};

//Forward declaration

//Rules
int rule_programm(symTable_t* table_sintax, stackAST_t* stack); //PROGRAM => TOKEN_START_TAG TOKEN_PROLOG COMMAND_OR_DECLARE_FUNCTION TOKEN_END_TAG
int command_or_declare_function(symTable_t* Table, stackAST_t* stack); // COMMAND_OR_DECLARE_FUNCTION
int delcare_function(symTable_t* Table, stackAST_t* stack); //rule num (DECLARE_FUNCTION)
int command(symTable_t* Table, stackAST_t* stack); //COMMAND
int command_variable(symTable_t* Table, stackAST_t* stack); //Variable
int command_if(symTable_t* Table, stackAST_t* stack);
int command_while(symTable_t* Table, stackAST_t* stack);
int command_call_function(symTable_t* Table, stackAST_t* stack);
int command_return(symTable_t* Table, stackAST_t* stack);
int call_function_or_expresion(symTable_t* Table, VariableType_t* RetrunType, stackAST_t* stack); //CALL_FUNCTION_OR_EXPRESION
int call_function(symTable_t* Table, VariableType_t* RetrunType, stackAST_t* stack);
int parametrs(symTable_t* Table, argumentsOfFunction_t* Arguments);
int term(symTable_t* Table, argumentsOfFunction_t* Arguments, func_par_t* FuncParam);
int term_without_epsilon(symTable_t* Table, argumentsOfFunction_t* Arguments, func_par_t* FuncParam);
int term_int();
int term_float();
int term_string();
int term_var_id();
int term_null();
int term_next(symTable_t* Table, argumentsOfFunction_t* Arguments, func_par_t* FuncParam);
int parametrs_without_epsilon(symTable_t* Table, argumentsOfFunction_t* Arguments);
int parametrs_next(symTable_t* Table, argumentsOfFunction_t* Arguments);

int data_type();
int func_type();




int parse(symTable_t* Table, ast_t* AST)
{
    //insert premade function


    if (AST == NULL)
    {
        freeSymTable(Table);
        disposeTree(AST);
        call_error(OTHERS_ERROR);
    }

    if (insertPremadeFunction(Table) == false)
    {
        disposeTree(AST);
        freeSymTable(Table);
        call_error(OTHERS_ERROR);
    }

    stackAST_t stack = initStackAST();
    if (pushStackAST(&stack, AST) == -1)
    {
        disposeTree(AST);
        freeSymTable(Table);
        call_error(OTHERS_ERROR);
    }

    scanner_result = get_token();
    scanner_result_is_processed = false;
    
    //Rule PROGRAM
    if (rule_programm(Table, &stack) == -1)
    {
        printf("fail\n");
        exit(-1);

    }
    
    //check if all function is defined
    if (isAllFunctionDefined(Table) == false)
    {
        disposeTree(AST);
        freeSymTable(Table);
        call_error(SEMANTIC_FUNC_ERROR);
    }

    return 1;
}

void get_unprocessed_token()
{
    if(scanner_result_is_processed)
    {
        free_token(scanner_result);
        scanner_result = get_token();
        scanner_result_is_processed = false;
    } 
}

//PROGRAM => TOKEN_PROLOG COMMAND_OR_DECLARE_FUNCTION TOKEN_END_TAG
int rule_programm(symTable_t* Table, stackAST_t* stack)
{
    printf("rule_program\n");

    //TOKEN_PROLOG
    get_unprocessed_token();

    if(scanner_result.type != TOKEN_PROLOG)
    {
        //Fail
        return -1;
    }
    scanner_result_is_processed = true;

    stack->top->LastCommand->thiscommand.prologSuccess = true;

    //call Rule COMMAND_OR_DECLARE_FUNCTION
    if(command_or_declare_function(Table,stack) != SUCCESS)
    {
        return -1;
    }

    //TOKEN_END_TAG
    get_unprocessed_token();

    if(scanner_result.type != TOKEN_END && scanner_result.type != TOKEN_EOF)
    {
        //Fail
        return -1;
    }
    scanner_result_is_processed = true;

    return 1;
}

int command_or_declare_function(symTable_t* Table, stackAST_t* stack)
{
    printf("comman_or_declare\n");

    int result = delcare_function(Table, stack);

    if (result == SUCCESS)
        return command_or_declare_function(Table, stack);

    if (result == FIAL_IN_MIDDLE)
    {
        
        freeSymTable(Table);
        call_error(SYNTAX_ERROR);
    }

    result = command(Table, stack);

    if (result == SUCCESS)
        return command_or_declare_function(Table, stack);
    else if (result == FIAL_IN_MIDDLE)
        return FIAL_IN_MIDDLE;


    return SUCCESS;
}


int delcare_function(symTable_t* Table, stackAST_t* stack)
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

    //ID_OF_FUNC
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_FUNC_ID)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    bool needToCheckArgumetns = false;
    
    symbol_t* NewFunction = findSymTableInCurentConxtext(Table, scanner_result.string);

    if (NewFunction == NULL)
    {
        NewFunction = initSymbol(TYPE_FUNCTION, scanner_result.string, NULL);
    }
    else
    {
        //Func already defined !!!
        if (NewFunction->info.function.defined == true)
        {
            freeSymTable(Table);
            call_error(SEMANTIC_FUNC_ERROR);
        }

        needToCheckArgumetns = true;
    }

    if (changeContext(Table, scanner_result.string) == false)
    {
        
        freeSymTable(Table);
        call_error(OTHERS_ERROR);
    }


    //(
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_L_PAR)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    //SymTable: arguments
    argumentsOfFunction_t NewArguments = initArguments();

    //PARAMETERS
    if(parametrs(Table, &NewArguments) != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    //check arguments !!!
    if (needToCheckArgumetns == true)
    {
        if (NewArguments.countOfArguments != NewFunction->info.function.arguments.countOfArguments)
        {
            freeSymTable(Table);
            call_error(SEMANTIC_FUNC_ERROR);
        }
    }

    NewFunction->info.function.arguments = NewArguments;

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

    switch(scanner_result.keyword)
    {
        case KEYWORD_FLOAT:
		case KEYWORD_Q_FLOAT:
		NewFunction->info.function.returnType = FLOAT_TYPE;
		break;

		case KEYWORD_INT:
		case KEYWORD_Q_INT:
		NewFunction->info.function.returnType = INT_TYPE;
		break;

		case KEYWORD_STRING:
		case KEYWORD_Q_STRING:
		NewFunction->info.function.returnType = STRING_TYPE;
		break;

        case KEYWORD_VOID:
        NewFunction->info.function.returnType = NULL_TYPE;
        break;

        default:
        break;
    }

    if (insertSymTable(Table, NewFunction) == false)
    {
        freeSymTable(Table);
        call_error(OTHERS_ERROR);
    }


    //TOKEN_L_BRAC 
    get_unprocessed_token();

    if (scanner_result.type != TOKEN_L_BRAC)
    {
        return -1;
    }

    scanner_result_is_processed = true;

    ast_t* StartOfFunc = createRootNode(false);
    if (StartOfFunc == NULL)
    {
        exit(-1);
    }
    ast_t* NewFuncAST = createDeclareFuncNode(NewFunction, StartOfFunc);
    if (NewFuncAST == NULL)
    {
        exit(-1);
    }

    addNextCommandToTop(stack, NewFuncAST);
    pushStackAST(stack, StartOfFunc);

    //COMMAND
    if(command(Table, stack) == FIAL_IN_MIDDLE)
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

    popStackAST(stack);

    if (NewFunction->info.function.returnType != NULL_TYPE && NewFunction->info.function.haveReturn == false)
    {
        freeSymTable(Table);
        call_error(SEMANTIC_PARAM_ERROR);
    }

    NewFunction->info.function.defined = true;
    changeContext(Table, NULL);
    
    return 1;
}

int command(symTable_t* Table, stackAST_t* stack)
{
    printf("command\n");
    int result;

    //Variable
    result = command_variable(Table, stack);
    if (result == 1)
    {
        result = command(Table, stack);
        return result != FIAL_IN_MIDDLE ? SUCCESS : result;
    }

    if (result == -1)
    {
        //!!! Error
        exit(-1);
    }

    //If
    result = command_if(Table,stack);
    if (result == 1)
    {
        result = command(Table, stack);
        return result != FIAL_IN_MIDDLE ? SUCCESS : result;
    }

    if (result == -1)
    {
        //!!! Error
        exit(-1);
    }

    //While
    result = command_while(Table, stack);
    if (result == 1)
    {
        result = command(Table, stack);
        return result != FIAL_IN_MIDDLE ? SUCCESS : result;
    }

    if (result == -1)
    {        
        freeSymTable(Table);
        call_error(SYNTAX_ERROR);
    }

    //Call function
    result = command_call_function(Table, stack);
    if (result == 1)
    {
        result = command(Table, stack);
        return result != FIAL_IN_MIDDLE ? SUCCESS : result;
    }

    if (result == -1)
    {
        freeSymTable(Table);
        call_error(SYNTAX_ERROR);
    }

    //return
    result = command_return(Table, stack);
    if (result == 1)
    {
        result = command(Table, stack);
        return result != FIAL_IN_MIDDLE ? SUCCESS : result;
    }

    if (result == -1)
    {
        freeSymTable(Table);
        call_error(SYNTAX_ERROR);
    }

    //epsilon
    return FAIL_IN_BEGIN;
}
//TOKEN_VAR_ID TOKEN_EQUAL CALL_FUNCTION_OR_EXPRESION TOKEN_SEMICOLON COMMAND
int command_variable(symTable_t* Table, stackAST_t* stack)
{
    printf("variable\n");
    //TOKEN_VAR_ID
    get_unprocessed_token();
    if (scanner_result.type != TOKEN_VAR_ID)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    symbol_t* NewVar = findSymTableInCurentConxtext(Table, scanner_result.string);

    if(NewVar == NULL)
    {
        NewVar = initSymbol(TYPE_VARIABLE, scanner_result.string, Table->CurentContext);
        
        if(NewVar == NULL)
        {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
        }
        insertSymTable(Table, NewVar);
    }

    ast_t* NewAssiamnet = createAssigmentFuncNode(NewVar, NULL);
    if (NewAssiamnet == NULL)
    {
        exit(-1);
    }

    addNextCommandToTop(stack, NewAssiamnet);

    //TOKEN_EQUAL
    get_unprocessed_token();
    if (scanner_result.type != TOKEN_EQUAL)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    //CALL_FUNCTION_OR_EXPRESION
    VariableType_t ReturnType = ERROR_TYPE;
    if (call_function_or_expresion(Table, &ReturnType, stack) != 1) //???
    {
        return FIAL_IN_MIDDLE;
    }
    
    scanner_result_is_processed = true; //EXPRESION!!!


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
int command_if(symTable_t* Table, stackAST_t* stack)
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
    int result = 0;
    ast_t* If_Exp = expresion(scanner_result, Table, &result);
    if (result != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    ast_t* TrueBlock = createRootNode(false);
    if (TrueBlock == NULL)
    {
        exit(-1);
    }
    ast_t* FalseBlock = createRootNode(false);
    if (FalseBlock == NULL)
    {
        exit(-1);
    }

    ast_t* If_Command = createIfNode(If_Exp, TrueBlock, FalseBlock);
    if (If_Command == NULL)
    {
        exit(-1);
    }

    addNextCommandToTop(stack, If_Command);

    pushStackAST(stack, TrueBlock);
    //TOKEN_L_BRAC
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    //COMMAND 
    if(command(Table, stack) == FIAL_IN_MIDDLE)
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

    popStackAST(stack);

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
    
    pushStackAST(stack, FalseBlock);

    //TOKEN_L_BRAC 
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    //COMMAND 
    if(command(Table, stack) == FIAL_IN_MIDDLE)
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

    popStackAST(stack);

    return SUCCESS;
}

//KEYWORD_WHILE TOKEN_L_PAR EXPRESSION TOKEN_R_PAR TOKEN_L_BRAC COMMAND TOKEN_R_BRAC COMMAND
int command_while(symTable_t* Table, stackAST_t* stack)
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
    int result = 0;
    ast_t* While_Exp = expresion(scanner_result, Table, &result);
    if (result != SUCCESS)
    {
        return FIAL_IN_MIDDLE;
    }

    ast_t* WhileBlock = createRootNode(false);
    if (WhileBlock == NULL)
    {
        exit(-1);
    }

    ast_t* While_Command = createWhileNode(While_Exp, WhileBlock);
    if (While_Command == NULL)
    {
        exit(-1);
    }
    addNextCommandToTop(stack, While_Command);

    pushStackAST(stack, WhileBlock);
    //  TOKEN_L_BRAC
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_BRAC)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true; 

    //  COMMAND 
    if(command(Table, stack) == FIAL_IN_MIDDLE)
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

    popStackAST(stack);

    return SUCCESS;
}
// CALL_FUNCTION TOKEN_SEMICOLON
int command_call_function(symTable_t* Table, stackAST_t* stack)
{
    printf("comand_call_func\n");
    // CALL_FUNCTION 
    VariableType_t ReturnType = ERROR_TYPE;
    int result = call_function(Table, &ReturnType, stack);
    
    
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
int command_return(symTable_t* Table, stackAST_t* stack)
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
    int result = 0;
    ast_t* Return_Exp = expresion(scanner_result, Table, &result);
    if (result == FIAL_IN_MIDDLE)
    {
        return FIAL_IN_MIDDLE;
    }
    if (Table->CurentContext != NULL)
    {
        symbol_t* Func = findSymTable(Table, Table->CurentContext, NULL);
        
        if (Func == NULL)
        {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
        }
        if(result == FAIL_IN_BEGIN)
        {
            if (Func->info.function.returnType != NULL_TYPE)
            {
                freeSymTable(Table);
                call_error(SEMANTIC_PARAM_ERROR);
            }

            Func->info.function.haveReturn = true;
        }
        else 
        {
            if (Func->info.function.returnType == NULL_TYPE)
            {
                freeSymTable(Table);
                call_error(SEMANTIC_RETURN_ERROR);
            }

            Func->info.function.haveReturn = true;
        }
    }
    
    //  TOKEN_SEMICOLON
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_SEMICOLON)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true; 

    ast_t* Return_Command = createReturnNode(Return_Exp);
    if(Return_Command == NULL)
    {
        exit(-1);
    }
    addNextCommandToTop(stack, Return_Command);

    return SUCCESS;
}

int call_function_or_expresion(symTable_t* Table, VariableType_t* RetrunType, stackAST_t* stack)
{
    printf("call_or_exp\n");
    if(call_function(Table, RetrunType, stack) != SUCCESS)
    {
        int result = 0;
        ast_t* Exp = expresion(scanner_result, Table, &result);
        if (Exp == NULL)
        {
            exit(-1);
        }

        if (stack->top->LastCommand->type == assigment_func 
        && stack->top->LastCommand->thiscommand.assigment_func.func == NULL)
        {
            stack->top->LastCommand->type = assigment_expression;
            stack->top->LastCommand->thiscommand.assigment_expression.target =
            stack->top->LastCommand->thiscommand.assigment_func.target;
            stack->top->LastCommand->thiscommand.assigment_expression.expression = Exp;
        }
        return result;
    }
    return SUCCESS;
}

// TOKEN_FUNC_ID TOKEN_L_PAR TERM TOKEN_R_PAR 
int call_function(symTable_t* Table, VariableType_t* RetrunType, stackAST_t* stack)
{
    printf("call\n");
    // TOKEN_FUNC_ID
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_FUNC_ID)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    symbol_t* NewFunc = findSymTable(Table, scanner_result.string, NULL);
    if (NewFunc == NULL)
    {
        NewFunc = initSymbol(TYPE_FUNCTION, scanner_result.string, NULL);
        NewFunc->info.function.defined = false;
        NewFunc->info.function.returnType = ERROR_TYPE;
    }

    // TOKEN_L_PAR 
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_L_PAR)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    argumentsOfFunction_t FuncArguments = initArguments();
    func_par_t* FuncParams = parInit();
    // TERM 
    if(term(Table, &FuncArguments, FuncParams) != SUCCESS)
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


    if(NewFunc->info.function.defined == true || findSymTable(Table, NewFunc->name, NULL) != NULL)
    {
        if (NewFunc->info.function.arguments.countOfArguments != FuncArguments.countOfArguments && NewFunc->info.function.arguments.countOfArguments != -1)
        {
            freeSymTable(Table);
            call_error(SEMANTIC_PARAM_ERROR);
        }
    }
    else
    {
        NewFunc->info.function.arguments.countOfArguments = FuncArguments.countOfArguments;
        if(insertSymTable(Table, NewFunc) == false)
        {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
        }

    }

    *RetrunType = NewFunc->info.function.returnType;

    if (stack->top->LastCommand->type == assigment_func && stack->top->LastCommand->thiscommand.assigment_func.func == NULL)
    {
        stack->top->LastCommand->thiscommand.assigment_func.func = NewFunc;
        stack->top->LastCommand->thiscommand.assigment_func.parameters = FuncParams;
    }
    else
    {
        ast_t* Call_Comand = createAssigmentFuncNode(NULL, NewFunc);
        Call_Comand->thiscommand.assigment_func.parameters = FuncParams;
        addNextCommandToTop(stack, Call_Comand);
    }

    return SUCCESS;
}

int term(symTable_t* Table, argumentsOfFunction_t* Arguments, func_par_t* FuncParam)
{
    printf("term\n");
    int result;
    result = term_without_epsilon(Table, Arguments, FuncParam);
    if (result == FAIL_IN_BEGIN || result == SUCCESS)
    {
        return SUCCESS;
    }


    return result;
}

int term_without_epsilon(symTable_t* Table, argumentsOfFunction_t* Arguments, func_par_t* FuncParam)
{
    printf("term_withput_eps\n");
    int result;

    result = term_int();
    if (result == SUCCESS)
    {
        if (addArgumentByKeyword(Arguments, KEYWORD_INT) == false)
        {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
        }
        
        long int* Value = malloc(sizeof(long int));
        *Value = scanner_result.int_value;
        addParametrer(FuncParam, NULL, Value, NULL, NULL);

        result = term_next(Table, Arguments, FuncParam);
        if (result != FIAL_IN_MIDDLE)
            return SUCCESS;
    }

    result = term_float();
    if(result == SUCCESS)
    {
        if (addArgumentByKeyword(Arguments, KEYWORD_FLOAT) == false)
        {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
        }


        double* Value = malloc(sizeof(double));
        *Value = scanner_result.float_value;
        addParametrer(FuncParam, NULL, NULL, Value, NULL);
        
        
        result = term_next(Table, Arguments, FuncParam);
        if (result != FIAL_IN_MIDDLE)
            return SUCCESS;
    }
     
    result = term_var_id();
    if(result == SUCCESS)
    {
        symbol_t* Var = findSymTableInCurentConxtext(Table, scanner_result.string);
        if( Var == NULL)
        {
            freeSymTable(Table);
            call_error(SEMANTIC_VAR_ERROR);
        }
        if (Var->type != TYPE_VARIABLE)
        {
            freeSymTable(Table);
            call_error(SEMANTIC_VAR_ERROR);
        }

        if (addArgumentByKeyword(Arguments, Var->info.variable.curentType) == false)
        {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
        }


        addParametrer(FuncParam, Var, NULL, NULL, NULL);

        result = term_next(Table, Arguments, FuncParam);
        if (result != FIAL_IN_MIDDLE)
            return SUCCESS;
    }

    result = term_string();
    if(result == SUCCESS)
    {
        if (addArgumentByKeyword(Arguments, KEYWORD_STRING) == false)
        {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
        }


        Dyn_String* Value = dyn_string_init();
        dyn_string_add_string(Value, scanner_result.string->string);
        addParametrer(FuncParam, NULL, NULL, NULL, Value);

        result = term_next(Table, Arguments, FuncParam);
        if (result != FIAL_IN_MIDDLE)
            return SUCCESS;
    }

    result = term_null();
    if(result == SUCCESS)
    {
         result = term_next(Table, Arguments, FuncParam);
        if (result != FIAL_IN_MIDDLE)
            return SUCCESS;
    }

    return FAIL_IN_BEGIN;
}

int term_next(symTable_t* Table, argumentsOfFunction_t* Arguments, func_par_t* FuncParam)
{
    printf("term_next\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_COMMA)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    
    if (term_without_epsilon(Table, Arguments, FuncParam) == FAIL_IN_BEGIN)
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

int parametrs(symTable_t* Table,argumentsOfFunction_t* Arguments)
{
    printf("parametrs\n");
    if (parametrs_without_epsilon(Table, Arguments) == FIAL_IN_MIDDLE)
    {
        return FIAL_IN_MIDDLE;
    }
    return SUCCESS;
}

int parametrs_without_epsilon(symTable_t* Table, argumentsOfFunction_t* Arguments)
{
    printf("parametrs_without_eps\n");
    //DATA_TYPE 
    if(data_type() != SUCCESS)
    {
        return FAIL_IN_BEGIN;
    }

    addArgumentByKeyword(Arguments, scanner_result.keyword);

    get_unprocessed_token();
    if(scanner_result.type != TOKEN_VAR_ID)
    {
        return FIAL_IN_MIDDLE;
    }
    scanner_result_is_processed = true;

    symbol_t* NewVar = findSymTableInCurentConxtext(Table, scanner_result.string);
    
    if(NewVar != NULL)
    {
        freeSymTable(Table);
        call_error(SEMANTIC_VAR_ERROR);
    }

    NewVar = initSymbol(TYPE_VARIABLE, scanner_result.string, Table->CurentContext);
    if (NewVar == NULL)
    {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
    }
    if (insertSymTable(Table, NewVar) == false)
    {
            freeSymTable(Table);
            call_error(OTHERS_ERROR);
    }

    int result = parametrs_next(Table, Arguments);
    if(result == FIAL_IN_MIDDLE)
    {
        return FIAL_IN_MIDDLE;
    }
    
    return SUCCESS;

}

int parametrs_next(symTable_t* Table, argumentsOfFunction_t* Arguments)
{
    printf("parametrs_next\n");
    get_unprocessed_token();
    if(scanner_result.type != TOKEN_COMMA)
    {
        return FAIL_IN_BEGIN;
    }
    scanner_result_is_processed = true;

    if(parametrs_without_epsilon(Table, Arguments) != SUCCESS)
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
        case KEYWORD_Q_INT:
        case KEYWORD_FLOAT:
        case KEYWORD_Q_FLOAT:
        case KEYWORD_STRING:
        case KEYWORD_Q_STRING:
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