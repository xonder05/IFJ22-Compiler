#include "code_gen_build.h"
#include "string.h"


inst_list_elem_ptr create_elem(block_type type,Dyn_String *code)
{
    inst_list_elem_ptr elem = malloc(sizeof(struct inst_list_elem));

    Dyn_String *string = dyn_string_init();
    // string ->alloc_size = code ->alloc_size;
    // string ->size = code ->size;

    dyn_string_add_string(string,code->string);

    elem->code = string;
    // elem->code  = code;
    elem->type = type;
    elem->previous = NULL;
    elem->next = NULL;
    return elem;
}

void instListInit(inst_list_t *list)
{
    list->first = NULL;
    list->last = NULL;
}

void instListInsertLast( inst_list_t *list, block_type type,Dyn_String *code)
{
    inst_list_elem_ptr elem = create_elem(type, code);


	if(list ->first == NULL)
	{
		list ->first = elem;
		list -> last = elem;
		return;
	}
    elem ->previous = list ->last;
    list-> last -> next= elem;
    list-> last = elem;
    elem ->next = NULL;
}

void instInsertBeforeWhile( inst_list_t *list, inst_list_elem_ptr elem)
{
    inst_list_elem_ptr tmp = list->last;
    // vracimse pred while
    while(tmp->type != while_loop_start && tmp ->type != while_loop_body && tmp != NULL)
    {
        tmp = tmp ->previous;
    }
    //chyba
    if(tmp == NULL)
    {
        return;
    }
    //insertu za tmp (tzn. pred while)
    elem ->previous = tmp;
    elem -> next = tmp ->next;
    tmp ->next = elem;
    elem ->next ->previous = elem;
}

void isntListDispose(inst_list_t *list)
{
    if(list->first == NULL)
    {
        return;
    }
    inst_list_elem_ptr elem = list ->last;
    while(elem != list ->first)
    {
        elem = elem -> previous;
        dyn_string_free(elem->next->code);
        free(elem->next);
    }
    dyn_string_free(list->first->code);
    free(list->first);
    list->first = NULL;
    list->last = NULL;
}

void printList(inst_list_t *list)
{
    inst_list_elem_ptr elem = list ->first;
    while(elem != NULL)
    {
        printf("%s",elem->code->string);
        elem = elem ->next;
    }
}

void copyListToString(inst_list_t *list, Dyn_String *string)
{
    inst_list_elem_ptr elem = list ->first;
    while(elem != NULL)
    {
        dyn_string_add_string(string,elem->code->string);
        elem = elem ->next;
    }

}


#define DEF_READS "############################### BUILT_IN ###############################\n\
LABEL $reads\n\
PUSHFRAME\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
READ LF@%retval1 string\n\
POPFRAME\n\
RETURN\n"

#define DEF_READI "LABEL $readi\n\
PUSHFRAME\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
READ LF@%retval1 int\n\
POPFRAME\n\
RETURN\n"

#define DEF_READF "LABEL $readf\n\
PUSHFRAME\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
READ LF@%retval1 float\n\
POPFRAME\n\
RETURN\n"

//musi se volat na kazdy argument zvlast
#define DEF_WRITE "LABEL $write\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
WRITE LF@param1\n\
POPFRAME\n\
RETURN\n"

#define DEF_FLOATVAL "label $floatval\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
DEFVAR LF@type_var\n\
TYPE LF@type_var LF@param1\n\
JUMPIFEQ $float1 LF@type_var string@string\n\
JUMPIFEQ $float2 LF@type_var string@float\n\
JUMPIFEQ $float3 LF@type_var string@int\n\
JUMPIFEQ $float4 LF@type_var string@bool\n\
JUMPIFEQ $float5 LF@type_var string@nil\n\
EXIT int@4\n\
LABEL $float1\n\
EXIT int@4\n\
LABEL $float2\n\
MOVE LF@%retval1 LF@param1\n\
JUMP $float_end\n\
LABEL $float3\n\
INT2FLOAT LF@%retval1 LF@param1\n\
JUMP $float_end\n\
LABEL $float4\n\
EXIT int@4\n\
LABEL $float5\n\
MOVE LF@%retval1 float@0x0p+0\n\
LABEL $float_end\n\
POPFRAME\n\
RETURN\n"

#define DEF_INTVAL "label $intval\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
DEFVAR LF@type_var\n\
TYPE LF@type_var LF@param1\n\
JUMPIFEQ $int1 LF@type_var string@string\n\
JUMPIFEQ $int2 LF@type_var string@float\n\
JUMPIFEQ $int3 LF@type_var string@int\n\
JUMPIFEQ $int4 LF@type_var string@bool\n\
JUMPIFEQ $int5 LF@type_var string@nil\n\
EXIT int@4\n\
LABEL $int1\n\
EXIT int@4\n\
LABEL $int2\n\
FLOAT2INT LF@%retval1 LF@param1\n\
JUMP $int_end\n\
LABEL $int3\n\
MOVE LF@%retval1 LF@param1\n\
JUMP $int_end\n\
LABEL $int4\n\
EXIT int@4\n\
LABEL $int5\n\
MOVE LF@%retval1 int@0\n\
LABEL $int_end\n\
POPFRAME\n\
RETURN\n"

#define DEF_STRING_VAL "label $stringval\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
DEFVAR LF@type_var\n\
TYPE LF@type_var LF@param1\n\
JUMPIFEQ $string1 LF@type_var string@string\n\
JUMPIFEQ $string2 LF@type_var string@float\n\
JUMPIFEQ $string3 LF@type_var string@int\n\
JUMPIFEQ $string4 LF@type_var string@bool\n\
JUMPIFEQ $string5 LF@type_var string@nil\n\
EXIT int@4\n\
LABEL $string1\n\
MOVE LF@%retval1 LF@param1\n\
JUMP $string_val_end\n\
LABEL $string2\n\
EXIT int@4\n\
LABEL $string3\n\
EXIT int@4\n\
LABEL $string4\n\
EXIT int@4\n\
LABEL $string5\n\
MOVE LF@%retval1 string@\n\
LABEL $string_val_end\n\
POPFRAME\n\
RETURN\n"

#define DEF_STRLEN "label $strlen\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 int@0\n\
DEFVAR LF@string_type\n\
TYPE LF@string_type LF@param1\n\
JUMPIFEQ $string_end LF@string_type string@string\n\
EXIT int@4\n\
LABEL $string_end\n\
STRLEN LF@%retval1 LF@param1\n\
POPFRAME\n\
RETURN\n"

#define DEF_SUBSTRING "label $substring\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@param2\n\
MOVE LF@param2 nil@nil\n\
MOVE LF@param2 LF@%2\n\
DEFVAR LF@param3\n\
MOVE LF@param3 nil@nil\n\
MOVE LF@param3 LF@%3\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
#body\n\
DEFVAR LF@sub_rel\n\
LT LF@sub_rel LF@param2 int@0 #zacatek par mensi nez 0\n\
JUMPIFEQ $substring_end LF@sub_rel bool@true\n\
LT LF@sub_rel LF@param3 int@0 #konec mensi nez 0\n\
JUMPIFEQ $substring_end LF@sub_rel bool@true\n\
GT LF@sub_rel LF@param2 LF@param3 #konec pred zacatkem\n\
JUMPIFEQ $substring_end LF@sub_rel bool@true\n\
\n\
DEFVAR LF@tmp_len\n\
STRLEN LF@tmp_len LF@param1\n\
SUB LF@tmp_len LF@tmp_len int@1\n\
GT LF@sub_rel LF@param2 LF@tmp_len\n\
JUMPIFEQ $substring_end LF@sub_rel bool@true\n\
\n\
STRLEN LF@tmp_len LF@param1\n\
GT LF@sub_rel LF@param3 LF@tmp_len\n\
JUMPIFEQ $substring_end LF@sub_rel bool@true\n\
\n\
MOVE LF@%retval1 string@\n\
DEFVAR LF@sub_char\n\
LABEL $substr_loop\n\
JUMPIFEQ $substring_end LF@param2 LF@param3\n\
GETCHAR LF@sub_char LF@param1 LF@param2\n\
CONCAT LF@%retval1 LF@%retval1 LF@sub_char \n\
ADD LF@param2 LF@param2 int@1\n\
JUMP $substr_loop\n\
\n\
\n\
LABEL $substring_end\n\
#body\n\
POPFRAME\n\
RETURN\n"

#define DEF_ORD "label $ord\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 int@0\n\
#body\n\
\n\
DEFVAR LF@ord_type\n\
TYPE LF@ord_type LF@param1\n\
JUMPIFEQ $ord_fine LF@ord_type string@string\n\
EXIT int@8\n\
\n\
LABEL $ord_fine\n\
DEFVAR LF@ord_len\n\
STRLEN LF@ord_len LF@param1\n\
JUMPIFEQ $ord_end LF@ord_len int@0\n\
\n\
STRI2INT LF@%retval1 LF@param1 int@0 \n\
\n\
LABEL $ord_end\n\
#body\n\
POPFRAME\n\
RETURN\n"

#define DEF_CHR "label $chr\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 string@\n\
#body\n\
DEFVAR LF@chr_type\n\
TYPE LF@chr_type LF@param1\n\
JUMPIFEQ $chr_fine LF@chr_type string@int\n\
EXIT int@4\n\
\n\
LABEL $chr_fine\n\
INT2CHAR LF@%retval1 LF@param1\n\
#body\n\
POPFRAME\n\
RETURN\n\
############################### BUILT_IN ###############################\n"


void def_built_in(inst_list_t *list)
{
    Dyn_String *string = dyn_string_init();
    Dyn_String *empty = dyn_string_init();

    dyn_string_add_string(string,DEF_READS);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);


    dyn_string_add_string(string,DEF_READI);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_READF);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_WRITE);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_FLOATVAL);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_INTVAL);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_STRING_VAL);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_STRLEN);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_SUBSTRING);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_ORD);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_CHR);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);



    dyn_string_free(string);
    dyn_string_free(empty);
}