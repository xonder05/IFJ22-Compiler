#include "dyn_string.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STR_LEN_INC 10

Dyn_String *dyn_string_init()
{
    Dyn_String *dyn_str = malloc(sizeof(Dyn_String));
    if(dyn_str==NULL)
    {
        return NULL;
    }


    dyn_str->string = (char*) malloc(STR_LEN_INC);
    if(dyn_str->string==NULL)
    {
        return NULL;
    }
    dyn_str->size = 0;
    dyn_str ->alloc_size = STR_LEN_INC;
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

bool dyn_string_equal(Dyn_String* String1, Dyn_String* String2)
{
    if (String1 == NULL && String2 == NULL)
        return true;

    if(String1 == NULL || String2 == NULL || String1->size != String2 ->size)
        return false;

    for(unsigned i = 0; i < String1->size; i++)
    {
        
        if(String1->string[i] != String2->string[i])
            return false;
    }

    return true;
}

bool dyn_string_add_char(Dyn_String *dyn_str, char c)
{
    if (dyn_str->size + 1 >= dyn_str->alloc_size)
    {   
        unsigned new_size = dyn_str->alloc_size + STR_LEN_INC;
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
    if (dyn_str->size + string_length + 1>= dyn_str->alloc_size)
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
    dyn_str->size = strlen(dyn_str->string);
    return true;
}

// void dyn_string_dup(Dyn_String *source, Dyn_String *destination)
// {

// }

int hex_to_dec(char hex[3])
{
    int decimal = 0;
    int base = 1;
    for(int i =1; i >= 0; i--)
    {
        if(hex[i] >= '0' && hex[i] <= '9')
        {
            decimal += (hex[i] - 48) * base;
            base *= 16;
        }
        else if(hex[i] >= 'A' && hex[i] <= 'F')
        {
            decimal += (hex[i] - 55) * base;
            base *= 16;
        }
        else if(hex[i] >= 'a' && hex[i] <= 'f')
        {
            decimal += (hex[i] - 87) * base;
            base *= 16;
        }
        else
        {
            return false;
        }
    }
    
    return decimal;

}

int oct_to_dec(char hex[4])
{
    int decimal = 0,octal,remain,i=0;
    octal = atoi(hex);
    while(octal != 0)
    {
        remain = octal % 10;
        decimal = decimal + (remain * pow_int(8,i++));
        octal = octal/10;
    }
    return decimal;
}

int pow_int (int base, int exp)  
{  
    int power = 1, i; 
    for (i = 1; i <= exp; ++i)  
    {  
        power = power * base;  
          
    }  
    return power;
} 

void unsigned_int_to_string(Dyn_String *old,int num)
{
    Dyn_String *string = dyn_string_init();

    if(num < 0)
    {
        return;
    }

    int remainder;
    while(1)
    {
        remainder = num % 10;
        num = num / 10;
        dyn_string_add_char(string,'0'+remainder);
        if(num == 0)
        {
            break;
        }
    }
    dyn_string_add_string(old,string->string);
    dyn_string_free(string);

}
