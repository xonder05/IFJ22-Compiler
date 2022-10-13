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


enum Token_type
{
    TOKEN_BLANK0,
    TOKEN_BLANK1,
    TOKEN_BLANK2,
    TOKEN_BLANK3,
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
    TOKNE_NOT_EQUAL,    // !==
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

};

// TOKEN
typedef struct token{
    enum Token_type type;
    union {
        int int_value;
        float float_value;
    };
} token_t;