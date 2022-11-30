#include "error.h"
#include <stdio.h>
#include <stdlib.h>


void call_error(ERROR_TYPE_t Error)
{
    fprintf(stderr, "%d", Error);
    exit(Error);
}