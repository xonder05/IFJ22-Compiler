#include "code_gen_build.h"


inst_list_elem_ptr create_elem(block_type type,Dyn_String *code)
{
    inst_list_elem_ptr elem = malloc(sizeof(struct inst_list_elem));
    elem->code = code;
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

void instListInsertLast( inst_list_t *list, inst_list_elem_ptr elem)
{
	if(list ->first == NULL)
	{
		list ->first = elem;
		list -> last = elem;
		return;
	}
    list-> last -> next= elem;
    list-> last = elem;
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
    inst_list_elem_ptr elem = list ->last;
    while(elem != list ->first)
    {
        elem = elem -> previous;
        dyn_string_free(elem->next->code);
        free(elem->next);
    }
    free(list->first);
    list->first = NULL;
    list->last = NULL;
}

#define def_reads "LABEL $reads\n\
PUSHFRAME\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
READ LF@%retval1 string\n\
POPFRAME\n\
RETURN\n"

#define def_readi "LABEL $readi\n\
PUSHFRAME\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
READ LF@%retval1 int\n\
POPFRAME\n\
RETURN\n"

#define def_readf "LABEL $readf\n\
PUSHFRAME\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 nil@nil\n\
READ LF@%retval1 float\n\
POPFRAME\n\
RETURN\n"

//musi se volat na kazdy argument zvlast
#define def_write "LABEL $write\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
WRITE LF@param1\n\
POPFRAME\n\
RETURN\n"

#define def_floatval "label $floatval\n\
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
EXIT int@8\n\
LABEL $float1\n\
EXIT int@8\n\
LABEL $float2\n\
MOVE LF@%retval1 LF@param1\n\
JUMP $float_end\n\
LABEL $float3\n\
INT2FLOAT LF@%retval1 LF@param1\n\
JUMP $float_end\n\
LABEL $float4\n\
EXIT int@8\n\
LABEL $float5\n\
MOVE LF@%retval1 float@0x0p+0\n\
LABEL $float_end\n\
POPFRAME\n\
RETURN"

#define def_intval "label $intval\n\
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
EXIT int@8\n\
LABEL $int1\n\
EXIT int@8\n\
LABEL $int2\n\
FLOAT2INT LF@%retval1 LF@param1\n\
JUMP $int_end\n\
LABEL $int3\n\
MOVE LF@%retval1 LF@param1\n\
JUMP $int_end\n\
LABEL $int4\n\
EXIT int@8\n\
LABEL $int5\n\
MOVE LF@%retval1 int@0\n\
LABEL $int_end\n\
POPFRAME\n\
RETURN"

#define def_stringval "label $stringval\n\
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
EXIT int@8\n\
LABEL $string1\n\
MOVE LF@%retval1 LF@param1\n\
JUMP $string_end\n\
LABEL $string2\n\
EXIT int@8\n\
LABEL $string3\n\
EXIT int@8\n\
LABEL $string4\n\
EXIT int@8\n\
LABEL $string5\n\
MOVE LF@%retval1 string@\n\
LABEL $string_end\n\
POPFRAME\n\
RETURN"

#define def_strlen "label $strlen\n\
PUSHFRAME\n\
DEFVAR LF@param1\n\
MOVE LF@param1 nil@nil\n\
MOVE LF@param1 LF@%1\n\
DEFVAR LF@%retval1\n\
MOVE LF@%retval1 int@0\n\
DEFVAR LF@string_type\n\
TYPE LF@string_type LF@param1\n\
JUMPIFEQ $string_end LF@string_type string@string\n\
EXIT int@8\n\
LABEL $string_end\n\
STRLEN LF@%retval1 LF@param1\n\
POPFRAME\n\
RETURN"

#define def_substring "label $substring\n\
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
RETURN"

#define def_ord "label $ord\n\
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
RETURN"

#define def_chr "label $chr\n\
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
EXIT int@8\n\
\n\
LABEL $chr_fine\n\
INT2CHAR LF@%retval1 LF@param1\n\
#body\n\
POPFRAME\n\
RETURN"
