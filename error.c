/*************************************************
* 
*   file name: error.c
*   VUT FIT Project Compiler for language IFJ22
*   Author: Aleksandr Kasinova xkasia01
*   Description: Error handling
*
*************************************************/

#include "error.h"
#include <stdio.h>
#include <stdlib.h>


void call_error(ERROR_TYPE_t Error)
{
    fprintf(stderr, "%d", Error);
    exit(Error);
}