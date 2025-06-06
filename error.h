/*************************************************
* 
*   file name: error.h
*   VUT FIT Project Compiler for language IFJ22
*   Author: Aleksandr Kasinova xkasia01
*   Description: Error handling
*
*************************************************/

#pragma once

typedef enum ERROR_TYPE
{
    LEX_ERROR = 1,
    SYNTAX_ERROR = 2,
    SEMANTIC_FUNC_ERROR = 3,
    SEMANTIC_PARAM_ERROR = 4,
    SEMANTIC_VAR_ERROR = 5,
    SEMANTIC_RETURN_ERROR = 6,
    SEMANTIC_COMPABILITY_ERROR = 7,
    SEMNATIC_OTHER_ERROR =8,
    OTHERS_ERROR = 99
}ERROR_TYPE_t;


void call_error(ERROR_TYPE_t Error);