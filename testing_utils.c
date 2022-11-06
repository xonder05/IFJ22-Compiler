#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "testing_utils.h"

// bude tezke funckce
void print_token(token_t *token)
{

    if (token->type == TOKEN_VAR_ID)
    {
        printf("TOKEN_VAR_ID, value =  %s\n",token->string->string);
    }
    else if (token->type == TOKEN_STRING)
    {
        printf("TOKEN_STRING, value = %s\n",token->string->string);
    }
    else if (token->type == TOKEN_FUNC_ID)
    {
        printf("TOKEN_FUNC_ID, value = %s\n",token->string->string);
    }
    else if(token->type == TOKEN_KEYWORD)
    {
        if(token->keyword == KEYWORD_ELSE) printf("TOKEN_KEYWORD, value = KEYWORD_ELSE\n");
        else if(token->keyword == KEYWORD_FLOAT) printf("TOKEN_KEYWORD, value = KEYWORD_FLOAT\n");
        else if(token->keyword == KEYWORD_Q_FLOAT) printf("TOKEN_KEYWORD, value = KEYWORD_Q_FLOAT\n");
        else if(token->keyword == KEYWORD_FUNCTION) printf("TOKEN_KEYWORD, value = KEYWORD_FUNCTION\n");
        else if(token->keyword == KEYWORD_IF) printf("TOKEN_KEYWORD, value = KEYWORD_IF\n");
        else if(token->keyword == KEYWORD_INT) printf("TOKEN_KEYWORD, value = KEYWORD_INT\n");
        else if(token->keyword == KEYWORD_Q_INT) printf("TOKEN_KEYWORD, value = KEYWORD_Q_INT\n");
        else if(token->keyword == KEYWORD_NULL) printf("TOKEN_KEYWORD, value = KEYWORD_NULL\n");
        else if(token->keyword == KEYWORD_RETURN) printf("TOKEN_KEYWORD, value = KEYWORD_RETURN\n");
        else if(token->keyword == KEYWORD_STRING) printf("TOKEN_KEYWORD, value = KEYWORD_STRING\n");
        else if(token->keyword == KEYWORD_Q_STRING) printf("TOKEN_KEYWORD, value = KEYWORD_Q_STRING\n");
        else if(token->keyword == KEYWORD_VOID) printf("TOKEN_KEYWORD, value = KEYWORD_VOID\n");
        else if(token->keyword == KEYWORD_WHILE) printf("TOKEN_KEYWORD, value = KEYWORD_WHILE\n");   
    }
    else if(token->type == TOKEN_END_TAG) printf("TOKEN_END_TAG\n");
    else if(token->type == TOKEN_SEMICOLON) printf("TOKEN_SEMICOLON\n");
    else if(token->type == TOKEN_COLON) printf("TOKEN_COLON\n");
    else if(token->type == TOKEN_COMMA) printf("TOKEN_COMMA\n");
    else if(token->type == TOKEN_DOT) printf("TOKEN_DOT\n");
    else if(token->type == TOKEN_BIGGER) printf("TOKEN_BIGGER\n");
    else if(token->type == TOKEN_BIGGER_EQ) printf("TOKEN_BIGGER_EQ\n");
    else if(token->type == TOKEN_SMALL_EQ) printf("TOKEN_SMALL_EQ\n");
    else if(token->type == TOKEN_START_TAG) printf("TOKEN_START_TAG\n");
    else if(token->type == TOKEN_EQUAL) printf("TOKEN_EQUAL\n");
    else if(token->type == TOKEN_EQUAL2) printf("TOKEN_EQUAL2\n");
    else if(token->type == TOKEN_NOT_EQUAL) printf("TOKEN_NOT_EQUAL\n");
    else if(token->type == TOKEN_L_PAR) printf("TOKEN_L_PAR\n");
    else if(token->type == TOKEN_R_PAR) printf("TOKEN_R_PAR\n");
    else if(token->type == TOKEN_L_BRAC) printf("TOKEN_L_BRAC\n");
    else if(token->type == TOKEN_R_BRAC) printf("TOKEN_R_BRAC\n");
    else if(token->type == TOKEN_PLUS) printf("TOKEN_PLUS\n");
    else if(token->type == TOKEN_MINUS) printf("TOKEN_MINUS\n");
    else if(token->type == TOKEN_MULTIPLY) printf("TOKEN_MULTIPLY\n");
    else if(token->type == TOKEN_DIVIDE) printf("TOKEN_DIVIDE\n");
    else if(token->type == TOKEN_PROLOG) printf("TOKEN_PROLOG\n");
    else if(token->type == TOKEN_PROLOG_FAIL) printf("TOKEN_PROLOG_FAIL\n");


    else if(token->type == TOKEN_INT)printf("TOKEN_INT, value: %ld\n",token->int_value);    
    else if(token->type == TOKEN_FLOAT)printf("TOKEN_FLOAT, value: %lf\n",token->float_value);    
    else if(token->type == TOKEN_EOF)printf("TOKEN_EOF\n");
    else if(token->type == TOKEN_EOF_FAIL)printf("TOKEN_EOF_FAIL\n");


    else if(token->type == TOKEN_BLANK0)printf("TOKEN_BLANK0\n");    
    else if(token->type == TOKEN_BLANK1)printf("TOKEN_BLANK1\n");    
    else if(token->type == TOKEN_BLANK2)printf("TOKEN_BLANK2\n");    
    else if(token->type == TOKEN_BLANK3)printf("TOKEN_BLANK3\n");    


}

