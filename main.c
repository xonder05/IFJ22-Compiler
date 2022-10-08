#include<stdio.h>
#include"declarations.h"

int main ()
{

    printf("test\n");

    codegen();

    lex();

    syntax();

    semantic();



    return 0;

}