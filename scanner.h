#pragma once

#include "dyn_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// list of keywords
typedef enum
{
	KEYWORD_ELSE,
	KEYWORD_FLOAT,
	KEYWORD_FUNCTION,
	KEYWORD_IF,
	KEYWORD_INT,
	KEYWORD_NULL,
	KEYWORD_RETURN,
	KEYWORD_STRING,
	KEYWORD_VOID,
	KEYWORD_WHILE,
} Keyword;


typedef enum
{
    
    TOKEN_VAR_ID,       // $smthing
    TOKEN_QUEST_MARK,   // ?
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
    TOKEN_STRING,       // "xxxx"
    TOKEN_FUNC_ID,      // id()  (NENI V AUTOMATU)
    TOKEN_KEYWORD,      // if,...   (NENI V AUTOMATU)
    TOKEN_INT,          // INT
    TOKEN_FLOAT,        // FLOAT
    TOKEN_EOF,          // EOF

    TOKEN_BLANK0,       // errors
    TOKEN_BLANK1,
    TOKEN_BLANK2,
    TOKEN_BLANK3,

    TOKEN_PROLOG,   // declare(strict_types=1);
    TOKEN_PROLOG_FAIL,  // anything else

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
    STATE_ID,            // keyword or function id  (NENI V AUTOMATU)
    STATE_INT,          // INT
    STATE_FLOAT,        // FLOAT
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

// TOKEN struct
typedef struct token{
    Token_type type; // enum
    union {
        long int int_value;
        double float_value;
        Dyn_String *string;
        Keyword keyword;
    };
} token_t;

// return next token (struct, so NO POINTER) 
// if token contain *string value, string has to be fried with dyn_string_free(token.string)
// number of token (starting from 0), just for proper scanning of prolog
token_t get_token(int token_num);

// deals with the "declare(static_type=1);"
// returns either TOKEN_PROLOG or TOKEN_PROLOG_FAIL
token_t deal_with_prolog();


// free dynamic string strored inside of token_t struct
void free_token(token_t token);