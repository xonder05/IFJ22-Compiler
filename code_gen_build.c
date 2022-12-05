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

// void instInsertBeforeWhile( inst_list_t *list, inst_list_elem_ptr elem)
// {
//     inst_list_elem_ptr tmp = list->last;
//     // vracimse pred while
//     while(tmp->type != while_loop_start && tmp != NULL)
//     {
//         tmp = tmp ->previous;
//     }
//     //chyba
//     if(tmp == NULL)
//     {
//         return;
//     }
//     //insertu za tmp (tzn. pred while)
//     elem ->previous = tmp;
//     elem -> next = tmp ->next;
//     tmp ->next = elem;
//     elem ->next ->previous = elem;
// }

void instInsertBeforeWhile( inst_list_t *list, block_type type, Dyn_String *code)
{

    inst_list_elem_ptr tmp = list->last;
    if(list ->first == NULL)
	{
        inst_list_elem_ptr elem = create_elem(type, code);
		list ->first = elem;
		list -> last = elem;
		return;
	}

    while(tmp != NULL && tmp->type != while_loop_start && tmp->type != if_cond)
    {
        tmp = tmp -> previous;
    }
    
    if(tmp == NULL)//nejseme ve while
    {
        instListInsertLast(list,type,code);
    }
    //nasli jsme while start
    else
    {
        int while_count = 0;
        int if_count = 0;
        tmp = list -> last;
        while(tmp != NULL)
        {            
            if(tmp->type == while_loop_start)
            {
                while_count++;
            }
            if(tmp->type == while_loop_end)
            {
                while_count--;
            }
            if(tmp->type == if_cond)
            {
                if_count++;
            }            
            if(tmp->type == if_false)
            {
                if_count--;
            }
            tmp = tmp -> previous;
        }
        tmp = list -> last;
        if(if_count < 0 || while_count < 0)
        {
            return;//error
        }
        while(if_count != 0 ||  while_count != 0)
        {

            if(tmp->type == if_cond)
            {
                if_count--;
            }
            if(tmp->type == while_loop_start)
            {
                while_count--;
            }
            tmp = tmp -> previous;

        }
        inst_list_elem_ptr elem = create_elem(type, code);
        //kdyz je while prvni v listu
        if(tmp->previous == NULL)
        {
            elem->next=list->first;
            tmp ->previous = elem;
            list -> first = elem;
        }        
        else//jinak
        {
            elem ->next = tmp;
            elem ->previous= tmp ->previous;
            elem -> previous ->next = elem;
            tmp ->previous = elem;
        }
    }


    return;   
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
RETURN\n"

#define DEF_CONC "label $&conc\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op2\n\
POPS LF@&op1\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFNEQ $&conc_end LF@&type1 LF@&type2\n\
JUMPIFNEQ $&conc_end LF@&type1 string@string\n\
CONCAT LF@&op1 LF@&op1 LF@&op2\n\
PUSHS LF@&op1\n\
POPFRAME\n\
RETURN\n\
label $&conc_end\n\
EXIT int@7\n"

#define DEF_EQUAL_TYPE "label $&equal_type\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op2\n\
POPS LF@&op1\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFEQ $&equal_type_type LF@&type1 LF@&type2\n\
PUSHS bool@false\n\
JUMP $&equal_type_end\n\
label $&equal_type_type\n\
EQ LF@&op1 LF@&op1 LF@&op2\n\
PUSHS LF@&op1\n\
label $&equal_type_end\n\
POPFRAME\n\
RETURN\n"

#define DEF_NOT_EQUAL_TYPE "label $&not_equal_type\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op2\n\
POPS LF@&op1\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFEQ $&not_equal_type_type LF@&type1 LF@&type2\n\
PUSHS bool@true\n\
JUMP $&not_equal_type_end\n\
label $&not_equal_type_type\n\
EQ LF@&op1 LF@&op1 LF@&op2\n\
NOT LF@&op1 LF@&op1\n\
PUSHS LF@&op1\n\
label $&not_equal_type_end\n\
POPFRAME\n\
RETURN\n"

#define DEF_GREATER_EQUAL "label $&greater_equal\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&bool1\n\
DEFVAR LF@&bool2\n\
POPS LF@&op2\n\
POPS LF@&op1\n\
EQ LF@&bool1 LF@&op1 LF@&op2 \n\
JUMPIFEQ $&greater_equal_end LF@&bool1 bool@true\n\
GT LF@&bool2 LF@&op1 LF@&op2 \n\
OR LF@&bool1 LF@&bool1 LF@&bool2 \n\
label $&greater_equal_end\n\
PUSHS LF@&bool1\n\
POPFRAME\n\
RETURN\n"

#define DEF_LESSER_EQUAL "label $&lesser_equal\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&bool1\n\
DEFVAR LF@&bool2\n\
POPS LF@&op2\n\
POPS LF@&op1\n\
EQ LF@&bool1 LF@&op1 LF@&op2\n\
JUMPIFEQ $&lesser_equal_end LF@&bool1 bool@true\n\
LT LF@&bool2 LF@&op1 LF@&op2 \n\
OR LF@&bool1 LF@&bool1 LF@&bool2 \n\
label $&lesser_equal_end\n\
PUSHS LF@&bool1\n\
POPFRAME\n\
RETURN\n"

#define DEF_IF_TO_RET "label $&ev_if_to_ret\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&typ1\n\
DEFVAR LF@&bool1\n\
MOVE LF@&bool1 bool@false\n\
DEFVAR LF@%retval1\n\
POPS LF@&op1\n\
TYPE LF@&typ1 LF@&op1 \n\
\n\
JUMPIFEQ $&ev_if_to_ret_int LF@&typ1 string@int\n\
JUMPIFEQ $&ev_if_to_ret_string LF@&typ1 string@string\n\
JUMPIFEQ $&ev_if_to_ret_nil LF@&typ1 string@nil\n\
JUMPIFEQ $&ev_if_to_ret_bool LF@&typ1 string@bool\n\
#kdyz jinej float tak chyba\n\
JUMPIFEQ $&ev_if_to_ret_end_false LF@&op1 float@0x0p+0\n\
EXIT int@7\n\
\n\
label $&ev_if_to_ret_int\n\
JUMPIFEQ $&ev_if_to_ret_end_false LF@&op1 int@0\n\
JUMP $&ev_if_to_ret_end_true\n\
\n\
label $&ev_if_to_ret_string\n\
JUMPIFEQ $&ev_if_to_ret_end_false LF@&op1 string@0\n\
#nedefinovana promena je chyba\n\
JUMPIFEQ $&ev_if_to_ret_end_false LF@&op1 string@\n\
JUMP $&ev_if_to_ret_end_true\n\
\n\
label $&ev_if_to_ret_nil\n\
JUMP $&ev_if_to_ret_end_false\n\
\n\
label $&ev_if_to_ret_bool\n\
JUMPIFEQ $&ev_if_to_ret_end_false LF@&op1 bool@false\n\
JUMP $&ev_if_to_ret_end_true\n\
\n\
label $&ev_if_to_ret_end_true\n\
MOVE LF@&bool1 bool@true\n\
label $&ev_if_to_ret_end_false\n\
MOVE LF@%retval1 LF@&bool1\n\
POPFRAME\n\
RETURN\n\
LABEL $&ev_if_to_ret_end_false_exit5\n\
EXIT int@5\n"

#define DEF_CONVERT_FOR_DIV "label $&convert_for_div\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op1\n\
POPS LF@&op2\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFEQ $&convert_for_div_par2 LF@&type1 string@float\n\
JUMPIFEQ $&convert_for_div_par1_int LF@&type1 string@int\n\
JUMPIFEQ $&convert_for_div_par1_nil LF@&type1 string@nil  \n\
EXIT int@7\n\
LABEL $&convert_for_div_par1_nil\n\
MOVE LF@&op1 float@0x0p+0\n\
JUMP $&convert_for_div_par2\n\
LABEL $&convert_for_div_par1_int\n\
INT2FLOAT LF@&op1 LF@&op1\n\
LABEL $&convert_for_div_par2\n\
JUMPIFEQ $&convert_for_div_end LF@&type2 string@float\n\
JUMPIFEQ $&convert_for_div_par2_int LF@&type2 string@int\n\
JUMPIFEQ $&convert_for_div_par2_nil LF@&type1 string@nil\n\
EXIT int@7\n\
LABEL $&convert_for_div_par2_nil\n\
MOVE LF@&op2 float@0x0p+0\n\
JUMP $&convert_for_div_end\n\
LABEL $&convert_for_div_par2_int\n\
INT2FLOAT LF@&op2 LF@&op2\n\
LABEL $&convert_for_div_end\n\
PUSHS LF@&op2\n\
PUSHS LF@&op1\n\
POPFRAME\n\
RETURN\n"

#define DEF_CONVERT_FOR_MATH "label $&convert_for_math\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op1\n\
POPS LF@&op2\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
#check for string and nill\n\
JUMPIFEQ $&convert_for_math_error LF@&type1 string@string\n\
JUMPIFEQ $&convert_for_math_error LF@&type2 string@string\n\
JUMPIFEQ $&convert_for_math_nil1 LF@&type1 string@nil\n\
label $&convert_for_math_back_nil1\n\
JUMPIFEQ $&convert_for_math_nil2 LF@&type2 string@nil\n\
label $&convert_for_math_back_nil2\n\
#both are numerical\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFEQ $&convert_for_math_end LF@&type1 LF@&type2\n\
#one is float the other int\n\
JUMPIFEQ $&convert_for_math_int1 LF@&type1 string@int\n\
INT2FLOAT LF@&op2 LF@&op2\n\
JUMP $&convert_for_math_end\n\
label $&convert_for_math_int1\n\
INT2FLOAT LF@&op1 LF@&op1\n\
label $&convert_for_math_end\n\
PUSHS LF@&op2\n\
PUSHS LF@&op1\n\
POPFRAME\n\
RETURN\n\
label $&convert_for_math_error\n\
EXIT int@7\n\
label $&convert_for_math_nil1\n\
MOVE LF@&op1 int@0\n\
JUMP $&convert_for_math_back_nil1\n\
label $&convert_for_math_nil2\n\
MOVE LF@&op2 int@0\n\
JUMP $&convert_for_math_back_ni2\n"

#define DEF_CONVERT_FOR_CONC "label $&convert_for_conc\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op1\n\
POPS LF@&op2\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFNEQ $&convert_for_conc_error LF@&type1 string@string\n\
JUMPIFNEQ $&convert_for_conc_error LF@&type1 LF@&type2 \n\
PUSHS LF@&op2\n\
PUSHS LF@&op1\n\
POPFRAME\n\
RETURN\n\
label $&convert_for_conc_error\n\
EXIT int@7\n"

#define CONVERT_FOR_GL "label $&convert_for_gl\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op1\n\
POPS LF@&op2\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFEQ $&convert_for_gl_end LF@&type1 LF@&type2\n\
JUMPIFEQ $&convert_for_gl_1nil LF@&type1 string@nil\n\
JUMPIFEQ $&convert_for_gl_2nil LF@&type2 string@nil\n\
JUMPIFEQ $&convert_for_gl_1string LF@&type1 string@string\n\
JUMPIFEQ $&convert_for_gl_2string LF@&type2 string@string\n\
JUMPIFEQ $&convert_for_gl_1int LF@&type1 string@int\n\
JUMPIFEQ $&convert_for_gl_2int LF@&type2 string@int\n\
#jinak nejaka chyba(nemelo by nastat)\n\
EXIT int@7\n\
\n\
\n\
label $&convert_for_gl_end\n\
PUSHS LF@&op2\n\
PUSHS LF@&op1\n\
POPFRAME\n\
RETURN\n\
#nilx || xnil\n\
label $&convert_for_gl_1nil\n\
label $&convert_for_gl_2nil\n\
EXIT int@7\n\
\n\
label $&convert_for_gl_1string\n\
label $&convert_for_gl_2string\n\
EXIT int@7\n\
\n\
label $&convert_for_gl_1int\n\
INT2FLOAT LF@&op1 LF@&op1\n\
JUMP $&convert_for_gl_end \n\
label $&convert_for_gl_2int\n\
INT2FLOAT LF@&op2 LF@&op2\n\
JUMP $&convert_for_gl_end\n"

#define CONVERT_FOR_GL_EQUAL "label $&convert_for_gl_equal\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op1\n\
POPS LF@&op2\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
JUMPIFEQ $&convert_for_gl_equal_end LF@&type1 LF@&type2\n\
JUMPIFEQ $&convert_for_gl_equal_1nil LF@&type1 string@nil\n\
JUMPIFEQ $&convert_for_gl_equal_2nil LF@&type2 string@nil\n\
JUMPIFEQ $&convert_for_gl_equal_1string LF@&type1 string@string\n\
JUMPIFEQ $&convert_for_gl_equal_2string LF@&type2 string@string\n\
JUMPIFEQ $&convert_for_gl_equal_1int LF@&type1 string@int\n\
JUMPIFEQ $&convert_for_gl_equal_2int LF@&type2 string@int\n\
#jinak nejaka chyba(nemelo by nastat)\n\
EXIT int@7\n\
\n\
label $&convert_for_gl_equal_end\n\
PUSHS LF@&op2\n\
PUSHS LF@&op1\n\
POPFRAME\n\
RETURN\n\
#nilx\n\
label $&convert_for_gl_equal_1nil\n\
JUMPIFEQ $&convert_for_gl_equal_1nil2int LF@&type2 string@int\n\
JUMPIFEQ $&convert_for_gl_equal_1nil2float LF@&type2 string@float\n\
#2 je string\n\
#skocim na pravdu kdyz je prazdny\n\
JUMPIFEQ $&convert_for_gl_equal_1nil2int0 lf@&op2 string@\n\
#jijnak null na ""\n\
MOVE LF@&op1 string@\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
\n\
#nilxint\n\
label $&convert_for_gl_equal_1nil2int\n\
JUMPIFEQ $&convert_for_gl_equal_1nil2int0 LF@&op2 int@0\n\
MOVE LF@&op2 bool@true\n\
MOVE LF@&op1 bool@false\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
#nilx0\n\
label $&convert_for_gl_equal_1nil2int0\n\
MOVE LF@&op1 int@1\n\
MOVE LF@&op2 int@1\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
#nilxfloat\n\
label $&convert_for_gl_equal_1nil2float\n\
JUMPIFEQ $&convert_for_gl_equal_1nil2float0 LF@&op2 float@0x0p+0\n\
MOVE LF@&op2 bool@true\n\
MOVE LF@&op1 bool@false\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
#nilx0.0\n\
label $&convert_for_gl_equal_1nil2float0\n\
MOVE LF@&op1 int@1\n\
MOVE LF@&op2 int@1\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
########dryhy je nil\n\
#Xnil\n\
label $&convert_for_gl_equal_2nil\n\
JUMPIFEQ $&convert_for_gl_equal_2nil1int LF@&type1 string@int\n\
JUMPIFEQ $&convert_for_gl_equal_2nil1float LF@&type1 string@float\n\
#1 je string\n\
#skocim na pravdu kdyz je prazdny\n\
JUMPIFEQ $&convert_for_gl_equal_2nil1int0 LF@&op1 string@\n\
#jijnak null na ""\n\
MOVE LF@&op2 string@\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
\n\
#intXnil\n\
label $&convert_for_gl_equal_2nil1int\n\
JUMPIFEQ $&convert_for_gl_equal_2nil1int0 LF@&op1 int@0\n\
MOVE LF@&op1 bool@true\n\
MOVE LF@&op2 bool@false\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
#0xnill\n\
label $&convert_for_gl_equal_2nil1int0\n\
MOVE LF@&op1 int@1\n\
MOVE LF@&op2 int@1\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
#floatxnil\n\
label $&convert_for_gl_equal_2nil1float\n\
JUMPIFEQ $&convert_for_gl_equal_2nil1float0 LF@&op1 float@0x0p+0\n\
MOVE LF@&op1 bool@true\n\
MOVE LF@&op2 bool@false\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
#0.0xnil\n\
label $&convert_for_gl_equal_2nil1float0\n\
MOVE LF@&op1 int@1\n\
MOVE LF@&op2 int@1\n\
JUMP $&convert_for_gl_equal_end\n\
\n\
label $&convert_for_gl_equal_1string\n\
label $&convert_for_gl_equal_2string\n\
EXIT int@7\n\
\n\
label $&convert_for_gl_equal_1int\n\
INT2FLOAT LF@&op1 LF@&op1\n\
JUMP $&convert_for_gl_equal_end \n\
label $&convert_for_gl_equal_2int\n\
INT2FLOAT LF@&op2 LF@&op2\n\
JUMP $&convert_for_gl_equal_end \n"

#define DEF_CONVERT "label $&convert\n\
CREATEFRAME\n\
PUSHFRAME\n\
DEFVAR LF@&op1\n\
DEFVAR LF@&op2\n\
DEFVAR LF@&type1\n\
DEFVAR LF@&type2\n\
POPS LF@&op1\n\
POPS LF@&op2\n\
TYPE LF@&type1 LF@&op1\n\
TYPE LF@&type2 LF@&op2\n\
#v budoucnu mozna pridat float na string operace etc.\n\
#stejne typ jsou\n\
JUMPIFEQ $&end_convert LF@&type1 LF@&type2\n\
#nejsou stejneho typu\n\
\n\
#uz nepravadim string na nic\n\
#JUMPIFEQ $&op2nastring LF@&type1 string@string\n\
#JUMPIFEQ $&op1nastring LF@&type2 string@string\n\
JUMPIFEQ $&op2nafloat LF@&type1 string@float\n\
JUMPIFEQ $&op1nafloat LF@&type2 string@float\n\
#JUMPIFEQ $&op1nafloat LF@&type1 string@bool\n\
#JUMPIFEQ $&op1nafloat LF@&type2 string@bool\n\
#nejakej error\n\
EXIT int@7\n\
\n\
LABEL $&op1nafloat\n\
INT2FLOAT LF@&op1 LF@&op1\n\
JUMP $&end_convert\n\
\n\
LABEL $&op2nafloat\n\
INT2FLOAT LF@&op2 LF@&op2\n\
JUMP $&end_convert\n\
\n\
LABEL $&op1nastring\n\
INT2CHAR LF@&op1 LF@&op1\n\
JUMP $&end_convert\n\
\n\
LABEL $&op2nastring\n\
INT2CHAR LF@&op2 LF@&op2\n\
JUMP $&end_convert\n\
\n\
LABEL $&end_convert\n\
PUSHS LF@&op2\n\
PUSHS LF@&op1\n\
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

    dyn_string_add_string(string,DEF_CONC);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_EQUAL_TYPE);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_NOT_EQUAL_TYPE);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_GREATER_EQUAL);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_LESSER_EQUAL);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_IF_TO_RET);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_CONVERT_FOR_DIV);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_CONVERT_FOR_MATH);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_CONVERT_FOR_CONC);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,CONVERT_FOR_GL);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,CONVERT_FOR_GL_EQUAL);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_add_string(string,DEF_CONVERT);
    instListInsertLast(list,func_beg,empty); instListInsertLast(list,func_body,string);instListInsertLast(list,func_end,empty);
    dyn_string_clear(string);

    dyn_string_free(string);
    dyn_string_free(empty);
}


void var_generated_init(var_generated_t *list)
{
    list->first = NULL;
}

var_gen_elem_ptr var_create_elem(Dyn_String *code)
{
    var_gen_elem_ptr elem = malloc(sizeof(struct var_gen_elem));

    Dyn_String *string = dyn_string_init();


    dyn_string_add_string(string,code->string);

    // elem->type = null;
    elem->code = string;
    elem->next = NULL;
    return elem;
}

void varListInsertLast( var_generated_t *list, Dyn_String *code)
{
    var_gen_elem_ptr elem = var_create_elem(code);


	if(list ->first == NULL)
	{
		list ->first = elem;
		return;
	}
    elem ->next = NULL;
    // elem->type = typ;
    var_gen_elem_ptr tmp = list->first;
    while(tmp->next != NULL)
    {
        tmp = tmp ->next;
    }
    tmp->next = elem;
}
void varListDispose(var_generated_t *list)
{
    if(list->first == NULL)
    {
        return;
    }
    var_gen_elem_ptr elem = list ->first;
    var_gen_elem_ptr next;
    while(elem != NULL)
    {
        next = elem ->next;
        dyn_string_free(elem->code);
        free(elem);
        elem = next;
    }
    list->first = NULL;
}
bool varListFind(var_generated_t *list,Dyn_String *name)
{
    var_gen_elem_ptr elem = list ->first;
    while(elem != NULL)
    {
        if(dyn_string_equal(elem->code,name))
        {
            return true;
        }
        elem = elem ->next;
    }
    return false;
}