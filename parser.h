#pragma once 

enum rule_result
{
    FAIL_IN_BEGIN = 0,
    FIAL_IN_MIDDLE = -1,
    SUCCESS = 1
};


int parse();

//Rules

//Start of Program
int program();


