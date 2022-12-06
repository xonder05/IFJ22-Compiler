#pragma once

#include "dyn_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/**
 * @brief list of keywords
 * 
 */
typedef enum
{
	KEYWORD_ELSE,
	KEYWORD_FLOAT,
	KEYWORD_Q_FLOAT,
	KEYWORD_FUNCTION,
	KEYWORD_IF,
	KEYWORD_INT,
	KEYWORD_Q_INT,
	KEYWORD_NULL,
	KEYWORD_RETURN,
	KEYWORD_STRING,
	KEYWORD_Q_STRING,
	KEYWORD_VOID,
	KEYWORD_WHILE,
} Keyword;


typedef enum
{
    
    TOKEN_VAR_ID,       // $smthing
    TOKEN_END_TAG,      // ?>
    TOKEN_SEMICOLON,    // ;
    TOKEN_COLON,        // :
    TOKEN_COMMA,        // ,
    TOKEN_DOT,          // .
    TOKEN_BIGGER,       // >
    TOKEN_BIGGER_EQ,    // >=
    TOKEN_SMALLER,      // <
    TOKEN_SMALL_EQ,     // <=
    TOKEN_START_TAG,    // <?
    TOKEN_EQUAL,        // =
    TOKEN_EQUAL2,       // ===
    TOKEN_NOT_EQUAL,    // !==
    TOKEN_L_PAR,        // (
    TOKEN_R_PAR,        // )
    TOKEN_L_BRAC,       // {
    TOKEN_R_BRAC,       // }
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_STRING,       // "xxxx"
    TOKEN_FUNC_ID,      // id()  (NENI V AUTOMATU)
    TOKEN_KEYWORD,      // if,...   (NENI V AUTOMATU)
    TOKEN_INT,          // INT
    TOKEN_FLOAT,        // FLOAT
    TOKEN_EOF,          // EOF
    TOKEN_EOF_FAIL,    

    TOKEN_END,

    TOKEN_BLANK0,       // errors
    TOKEN_BLANK1,
    TOKEN_BLANK2,
    TOKEN_BLANK3,

    TOKEN_PROLOG,   // declare(strict_types=1);
    TOKEN_PROLOG_FAIL,  // anything else

    STATE_START_PROLOG,
    STATE_SMALL_PROLOG,
    STATE_START_TAG_PHP_PROLOG,
    
}Token_type;

typedef enum
{
    STATE_VAR_ID,       // $smthing
    STATE_VAR_ID_START, // non finishing $
    STATE_QUEST_MARK,   // ?
    STATE_BIGGER,       // >
    STATE_SMALLER,      // <
    STATE_EQUAL,        // =      
    STATE_EQUAL1,       // == non finishing 
    STATE_NOT_EQUAL1,   // ! non finishing
    STATE_NOT_EQUAL2,   // != non 
    STATE_STRING_READ,  // "x" non finishing
    STATE_STRING_ESCAPE,  // "x\.." non finishing, not in fsm
    STATE_ID,            // keyword or function id  (NENI V AUTOMATU)
    STATE_INT,          // INT
    STATE_FLOAT,        // FLOAT
    STATE_FLOAT_EXP,        // FLOAT
    STATE_COMMENT,      // / non finishing
    STATE_LINE_COMMENT, // // non finishing
    STATE_BLOCK_COM,    // /* non finishing
    STATE_BLOCK_COM1,   // /* * non finishing
    STATE_START,         // just the start, non finishing
    STATE_BLANK0,
    STATE_BLANK1,
    STATE_BLANK2,
    STATE_BLANK3,

}State_t;

/**
 * @brief TOKEN struct
 * 
 */
typedef struct token{
    Token_type type; // enum
        long int int_value;
        double float_value;
        Dyn_String *string;
        Keyword keyword;
} token_t;



/**
 * return next token (struct, so NO POINTER)
 * if token contain *string value, string has to be fried with dyn_string_free(token.string)
 * number of token (starting from 0), just for proper scanning of prolog 
 */
token_t get_token();

/**
 * 
 * deals with the "declare(static_type=1);"
 * returns either TOKEN_PROLOG or TOKEN_PROLOG_FAIL
 * 
 */
token_t deal_with_prolog();

/**
 * called after ?>
 * returns either TOKEN_EOF or TOKEN_EOF_FAIL
 * after ?> cant be anything else than EOF or \nEOF
 * 
 */
bool deal_with_end();

/**
 * @brief free dynamic string strored inside of token_t struct
 * 
 * 
 */
void free_token(token_t token);