#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct{

    char *string;
    unsigned size;  //length of string
    unsigned alloc_size;    // how many chars are allocated

}Dyn_String;


// return ready-to-use structure
Dyn_String *dyn_string_init();

// delete string stored in struct
void dyn_string_clear(Dyn_String *dyn_str);

// free the struct
void dyn_string_free(Dyn_String *dyn_str);

// append one char at the end of string
bool dyn_string_add_char(Dyn_String *dyn_str, char c);

// append string c to dynamic string
bool dyn_string_add_string(Dyn_String *dyn_str, char *c);
