#include "symtable.h"

									
unsigned int ELFHash(Dyn_String* SymbolName) {
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < SymbolName->size; i++)
	{
	    unsigned int x = 0;
		hash = (hash << 4) + SymbolName->string[i];

		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
		}

		hash &= ~x;
	}
	return hash % SYMTABLE_MAX_SIZE;
}

symTable_t* initSymTable()
{
    size_t size = SYMTABLE_MAX_SIZE;
	symTable_t* table = malloc(size * sizeof(symTable_t));
	if (table == NULL) {
		return NULL;
	}
	table->allocated = size;
	table->size = 0;
	for (size_t i = 0; i < table->allocated; i++) 
	{
		table->array[i] = NULL;
	}
    table->CurentContext = NULL;
	return table;
}

bool changeContext(symTable_t* Table, Dyn_String* NewContext)
{
	if (Table->CurentContext != NULL)
	{
		dyn_string_free(Table->CurentContext);
	}

	if(NewContext == NULL)
	{
		Table->CurentContext = NULL;
		return true;
	}

	Dyn_String* Context = dyn_string_init();
	if (Context == NULL)
	{
		return false;
	}

    if (dyn_string_add_string(Context, NewContext->string) == false)
	{
		dyn_string_free(Table->CurentContext);
		return false;
	}

	Table->CurentContext = Context;

	return true;
}

void clearSymTable(symTable_t* Table)
{
    if (Table == NULL)
		return;

	for (size_t i = 0; i < Table->allocated; i++) 
    {
		symbol_t* current = Table->array[i];
		symbol_t* next = NULL;
		while (current != NULL) {
			next = current->next;
			freeSymbol(current);
			current = next;
		}
	}
}

void freeSymTable(symTable_t* Table)
{
    if (Table == NULL)
		return;

	clearSymTable(Table);
	free(Table);
}

void deleteSymTable(symTable_t* Table, Dyn_String* Name, Dyn_String* Context)
{
    if (Table == NULL || Name == NULL) 
    {
		return;
	}

	size_t index = ELFHash(Name);
	symbol_t* current = Table->array[index];
    symbol_t* previous = NULL;

	while (current != NULL) {
		if (dyn_string_equal(current->name, Name) && dyn_string_equal(current->context, Context)) 
        {
			if (previous != NULL) {
				previous->next = current->next;
			} else {
				Table->array[index] = NULL;
			}
			freeSymbol(current);
			Table->size--;
			return;
		}
		previous = current;
		current = current->next;
	}
}

symbol_t* findSymTableInCurentConxtext(symTable_t* Table, Dyn_String* Name)
{
    if (Table == NULL || Name == NULL)
		return NULL;

	size_t index = ELFHash(Name);
	symbol_t* current = Table->array[index];

	while (current != NULL) 
    {	
		if (dyn_string_equal(current->name, Name) && dyn_string_equal(current->context, Table->CurentContext))
				return current;

		current = current->next;
	}
	
	return NULL;
}

symbol_t* findSymTable(symTable_t* Table, Dyn_String* Name, Dyn_String* Context)
{
	if (Table == NULL || Name == NULL)
		return NULL;

	size_t index = ELFHash(Name);
	symbol_t* current = Table->array[index];


	while (current != NULL) 
    {	
		if (dyn_string_equal(current->name, Name) && dyn_string_equal(current->context, Context))
				return current;

		current = current->next;
	}
	
	return NULL;
}


bool insertSymTable(symTable_t* Table, symbol_t* Symbol)
{
    if (Table == NULL || Symbol == NULL) {
		return false;
	}
	

	size_t index = ELFHash(Symbol->name);
	symbol_t *current = Table->array[index];
    symbol_t *previous = NULL;


	
	while (current != NULL) {
		if (dyn_string_equal(current->name, Symbol->name) && dyn_string_equal(current->context, Symbol->context)) 
        {
			return false;
		}
		previous = current;
		current = current->next;
	}

	if (previous == NULL) 
    {
		Table->array[index] = Symbol;
		Table->size++;
	} 
    else
		previous->next = Symbol;

	return true;
}

bool isAllFunctionDefined(symTable_t* Table)
{
	if (Table == NULL)
		return true;

	for (size_t i = 0; i < Table->allocated; i++) 
    {
		symbol_t* current = Table->array[i];
		while (current != NULL) {
			if (current->type == TYPE_FUNCTION)
			{
				if (current->info.function.defined == false)
				{
					return false;
				}
			}
			current = current->next;
		}
	}
	return true;
}

symbol_t* initSymbol(symbolType_t Type, Dyn_String* Name, Dyn_String* Context)
{
	symbol_t *symbol = malloc(sizeof(symbol_t));
	if (symbol == NULL)
		return NULL;
    
    Dyn_String* SymbolName = dyn_string_init();
	if (SymbolName == NULL)
	{
		free(symbol);
		return NULL;
	}
    if (dyn_string_add_string(SymbolName, Name->string) == false)
	{
		dyn_string_free(SymbolName);
		free(symbol);
		return NULL;
	}

	symbol->name = SymbolName;

	symbol->next = NULL;
	symbol->type = Type;

	if (Context != NULL)
	{
		Dyn_String* SymbolContext = dyn_string_init();
		if (SymbolContext == NULL)
		{
			free(symbol);
			dyn_string_free(SymbolName);
			return NULL;
		}
		if (dyn_string_add_string(SymbolContext, Context->string) == false)
		{
			dyn_string_free(SymbolContext);
			dyn_string_free(SymbolName);
			free(symbol);
			return NULL;
		}
		symbol->context = SymbolContext;
	}
	else 
		symbol->context = NULL;

    if(Type == TYPE_FUNCTION)
    {
        symbol->info.function.arguments.countOfArguments = 0;
		symbol->info.function.arguments.TypesOfArguments = NULL;
		symbol->info.function.defined = false;
		symbol->info.function.returnType = ERROR_TYPE;
    }
    else 
    {
        symbol->info.variable.curentType = ERROR_TYPE;
    }

	return symbol;
}

void freeSymbol(symbol_t* Symbol)
{
    if (Symbol == NULL) {
		return;
	}
	if (Symbol->type == TYPE_FUNCTION) {
		dyn_string_free(Symbol->name);
        Symbol->name = NULL;
	}
	if (Symbol->context != NULL) {
		dyn_string_free(Symbol->context);
		Symbol->context = NULL;
	}
	if (Symbol->name != NULL) {
		dyn_string_free(Symbol->name);
		Symbol->name = NULL;
	}
	free(Symbol);
}

argumentsOfFunction_t initArguments()
{
	argumentsOfFunction_t arguments;

	arguments.countOfArguments = 0;
	arguments.TypesOfArguments = malloc(sizeof(VariableType_t) * 1);

	return arguments;
}

bool addArgumentByKeyword(argumentsOfFunction_t* Arguments, int Keyword)
{
	Arguments->TypesOfArguments = realloc(Arguments->TypesOfArguments, (sizeof(VariableType_t) * Arguments->countOfArguments + 1));
	if (Arguments->TypesOfArguments == NULL)
	{
		return false;
	}
	
	Arguments->countOfArguments++;

	switch (Keyword)
	{
		case KEYWORD_FLOAT:
		case KEYWORD_Q_FLOAT:
		Arguments->TypesOfArguments[Arguments->countOfArguments] = FLOAT_TYPE;
		break;

		case KEYWORD_INT:
		case KEYWORD_Q_INT:
		Arguments->TypesOfArguments[Arguments->countOfArguments] = INT_TYPE;
		break;

		case KEYWORD_STRING:
		case KEYWORD_Q_STRING:
		Arguments->TypesOfArguments[Arguments->countOfArguments] = STRING_TYPE;
		break;
	}
	return true;
}