#include "dyn_string.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define STR_LEM_INC 10

Dyn_String *dyn_string_init()
{
    Dyn_String *dyn_str = malloc(sizeof(Dyn_String));
    if(dyn_str==NULL)
    {
        return NULL;
    }


    dyn_str->string = (char*) malloc(STR_LEM_INC);
    if(dyn_str->string==NULL)
    {
        return NULL;
    }
    dyn_str->size = 0;
    dyn_str ->alloc_size = STR_LEM_INC;
    dyn_str->string[0] = '\0';
    return dyn_str;

}

void dyn_string_clear(Dyn_String *dyn_str)
{
    dyn_str->size = 0;
    dyn_str->string[0] = '\0';
}

void dyn_string_free(Dyn_String *dyn_str)
{
    free(dyn_str->string);
    free(dyn_str);
}

bool dyn_string_add_char(Dyn_String *dyn_str, char c)
{
    if (dyn_str->size + 1 >= dyn_str->alloc_size)
    {   
        unsigned new_size = dyn_str->alloc_size + STR_LEM_INC;
        dyn_str->string = (char*)realloc(dyn_str->string,new_size);
        if(dyn_str==NULL)
        {
            return false;
        }
        dyn_str->alloc_size = new_size;
    }

    dyn_str->string[dyn_str->size++]=c;
    dyn_str->string[dyn_str->size]='\0';
    return true;
}


bool dyn_string_add_string(Dyn_String *dyn_str, char *c)
{
    int string_length = strlen(c);
    if (dyn_str->size + string_length >= dyn_str->alloc_size)
    {   
        unsigned new_size = dyn_str->alloc_size + string_length;
        dyn_str->string = (char*)realloc(dyn_str->string,new_size);
        if(dyn_str==NULL)
        {
            return false;
        }
        dyn_str->alloc_size = new_size;
    }

    strcat(dyn_str->string,c);
    dyn_str->size = strlen(dyn_str->string) + 1;
    return true;
}



