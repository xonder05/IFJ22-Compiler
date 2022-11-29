#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "dyn_string.h"
#include "scanner.h"

#define SYMTABLE_MAX_SIZE 8191 // 2^13 - 1 

typedef struct symbol symbol_t;

typedef enum symbolType
{
    TYPE_FUNCTION,
    TYPE_VARIABLE
}symbolType_t;

typedef enum VariableType
{
    FLOAT_TYPE,
    INT_TYPE,
    STRING_TYPE,
    NULL_TYPE,
    ERROR_TYPE
} VariableType_t;

typedef struct argumentsOfFunction
{
    int countOfArguments;
    VariableType_t* TypesOfArguments;
} argumentsOfFunction_t;

typedef struct symbolFunctionInfo
{
    argumentsOfFunction_t arguments;
    bool defined;
    VariableType_t returnType;
} symbolFunctionInfo_t;

typedef struct symbolVariableInfo
{
    VariableType_t curentType;
} symbolVariableInfo_t;

typedef union symbolInfo {
	symbolFunctionInfo_t function;
	symbolVariableInfo_t variable;
} symbolInfo_t;

struct symbol {
	Dyn_String* name;
	symbolType_t type;
	symbolInfo_t info;
	Dyn_String* context;
	symbol_t* next;
};

typedef struct symTable {
	size_t size;
	size_t allocated;
    Dyn_String* CurentContext;
	symbol_t* array[];
} symTable_t;

//Table
unsigned int ELFHash(Dyn_String* SymbolName);

symTable_t* initSymTable();

bool changeContext(symTable_t* Table, Dyn_String* NewContext);

void clearSymTable(symTable_t* Table);

void freeSymTable(symTable_t* Table);

void deleteSymTable(symTable_t* Table, Dyn_String* Name, Dyn_String* Context);

symbol_t* findSymTableInCurentConxtext(symTable_t* Table, Dyn_String* Name);

symbol_t* findSymTable(symTable_t* Table, Dyn_String* Name, Dyn_String* Context);

bool insertSymTable(symTable_t* Table, symbol_t* Symbol);

bool isAllFunctionDefined(symTable_t* Table);

bool insertPremadeFunction(symTable_t* Table);

//Symbol
symbol_t* initSymbol(symbolType_t Type, Dyn_String* Name, Dyn_String* Context);

void freeSymbol(symbol_t* Symbol); //!!! update 

//Arguments
argumentsOfFunction_t initArguments();

bool addArgumentByKeyword(argumentsOfFunction_t* Arguments, int Keyword);