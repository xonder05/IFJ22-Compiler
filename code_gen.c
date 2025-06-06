/*************************************************
* 
*   file name: code_gen.c
*   VUT FIT Project Compiler for language IFJ22
*   Author: Ondřej Bahounek xbahou00
*   Description: Code generating from syntax tree
*
*************************************************/

#include "scanner.h"
#include "code_gen_build.h"
#include "code_gen.h"
#include "dyn_string.h"
#include "abstact_syntax_tree.h"
#include "error.h"
#include <string.h>

//returns 0 on succes, error code on failure
int gen_from_ass(ast_t *tree, inst_list_t *body_list,inst_list_t *func_list,int *if_count,var_generated_t *var_list){
    // printf("%d\n",tree->type);
        // printf("hmm");
    if(tree == NULL)
    {
        return 0;
    }
    if(&(tree->thiscommand) == NULL)
    {
        return 0;
    }
    switch (tree->type)
    {
    case root:
        if(tree->nextcommand == NULL)//jenom root
        {
            return 0;
        }
        return gen_from_ass(tree ->nextcommand,body_list,func_list,if_count,var_list);
        break;
    case assigment_expression:
        return gen_assig_expression(&(tree->thiscommand),body_list,var_list) + gen_from_ass(tree ->nextcommand,body_list,func_list,if_count,var_list);
        break;
    case assigment_func:
        return generate_func_asign(&(tree->thiscommand),body_list,var_list) + gen_from_ass(tree ->nextcommand,body_list,func_list,if_count,var_list);
        break;
    case declare_func:
        return generate_func(&(tree->thiscommand),func_list,if_count,var_list) + gen_from_ass(tree ->nextcommand,body_list,func_list,if_count,var_list);
        break;   
    case if_statement:
        return generate_if(&(tree->thiscommand),body_list,if_count,func_list,var_list) + gen_from_ass(tree ->nextcommand,body_list,func_list,if_count,var_list);
        break;
    case while_statement:
        return generate_while(&(tree->thiscommand),body_list,func_list,var_list,if_count) + gen_from_ass(tree ->nextcommand,body_list,func_list,if_count,var_list);
        break;
    case expression:
        return ev_expression(&(tree->thiscommand),body_list);
        break;
    case return_statement:
        return generate_return(&(tree->thiscommand),body_list);
        break;
    
    default:
        break;
    }
    return 0;
}

//returns 0 on succes, error code on failure
int generate(ast_t *tree)
{
    int error_code = 0;
    int if_count = 0;

    //create main dll
    inst_list_t main_list,main_body_list,func_list;
    instListInit(&main_list);
    instListInit(&main_body_list);
    instListInit(&func_list);

    var_generated_t var_list;
    var_generated_init(&var_list);

    //gen built-in
    def_built_in(&func_list);


    //go through tree, generate etc...
    error_code = gen_from_ass(tree,&main_body_list,&func_list,&if_count,&var_list);
    if(error_code != 0)
    {
        isntListDispose(&main_list);
        isntListDispose(&main_body_list);
        isntListDispose(&func_list);
        call_error(OTHERS_ERROR);
    }

    //insert header to main_list
    Dyn_String *header_string = dyn_string_init();
    dyn_string_add_string(header_string,M_HEADER);
    instListInsertLast(&main_list,final_header_block,header_string);


    //insert functions into the main list (copy from function_list)
    Dyn_String *function_string = dyn_string_init();
    copyListToString(&func_list,function_string);
    instListInsertLast(&main_list,final_function_block,function_string);


    //insert main label
    Dyn_String *main_label_string = dyn_string_init();
    dyn_string_add_string(main_label_string,M_MAIN);    
    instListInsertLast(&main_list,final_main_label_block,main_label_string);

    //insert main body  to main list
    Dyn_String *main_body_string = dyn_string_init();
    copyListToString(&main_body_list,main_body_string);   
    instListInsertLast(&main_list,final_main_body_block,main_body_string);

    //dispose now redundant lists
    isntListDispose(&main_body_list);
    isntListDispose(&func_list);
    //free Dyn_strings
    dyn_string_free(header_string);
    dyn_string_free(function_string);
    dyn_string_free(main_label_string);
    dyn_string_free(main_body_string);
    varListDispose(&var_list);

    printList(&main_list);
    isntListDispose(&main_list);
    return 0;
}

//TODO definice uvnitr fce
int generate_func(node_t *node, inst_list_t *func_list,int *if_count,var_generated_t *var_list)
{  
    int error_code = 0;

    // add func_beg to func list
    // func_beg = "LABEL $ContextName"
    //             PUSHFRAME
    //             DEFVAR LF@param1
    //             MOVE LF@param1 LF@%1
    //             .
    //             .
    //             DEFVAR LF@paramN
    //             MOVE LF@paramN LF@%N
    Dyn_String *label_string = dyn_string_init();
    dyn_string_add_string(label_string,"##############################################################FUNC START######################################################\n");
    dyn_string_add_string(label_string,"LABEL $");
    if(node->declare_func.func_name->context != NULL)
    {
        dyn_string_add_string(label_string,node->declare_func.func_name->context->string);
    }
    dyn_string_add_string(label_string,node->declare_func.func_name->name->string);
    dyn_string_add_char(label_string,'\n'); 
    dyn_string_add_string(label_string,"PUSHFRAME\n");
           

    dyn_string_add_string(label_string,"DEFVAR LF@%retval1\n");
    dyn_string_add_string(label_string,"MOVE LF@%retval1 nil@nil\n");
    


    if(node->declare_func.func_name->info.function.arguments.countOfArguments > 0)
    {

       for(int i = 1; i <= node->declare_func.func_name->info.function.arguments.countOfArguments;i++) 
       {

            // DEFVAR LF@param1
            dyn_string_add_string(label_string,"DEFVAR LF@param");
            unsigned_int_to_string(label_string,i);
            dyn_string_add_char(label_string,'\n'); 

            // MOVE LF@param1 LF@%1
            dyn_string_add_string(label_string,"MOVE LF@param");
            unsigned_int_to_string(label_string,i);
            dyn_string_add_string(label_string," LF@%");
            
            unsigned_int_to_string(label_string,i);
            dyn_string_add_char(label_string,'\n'); 

       }

        //check arguments type and actual params
        dyn_string_add_string(label_string,"###################PARAM TYPE CONTROL####################\n");
        dyn_string_add_string(label_string,"DEFVAR LF@type_var\n");

       for(int i = 1; i <= node->declare_func.func_name->info.function.arguments.countOfArguments;i++) 
        {
            //move type to type_var
            dyn_string_add_string(label_string,"MOVE LF@type_var ");
            switch (node->declare_func.func_name->info.function.arguments.TypesOfArguments[i])
            {
            case FLOAT_TYPE:
                dyn_string_add_string(label_string,"float@0x1p+0\n");
                break;
            case INT_TYPE:
                dyn_string_add_string(label_string,"int@6\n");
                break;
            case STRING_TYPE:
                dyn_string_add_string(label_string,"string@string\n");
                break;
            case NULL_TYPE:
                dyn_string_add_string(label_string,"nil@nil\n");
                break;         
            default:
                dyn_string_add_string(label_string,"nil@nil\n");
                break;
            }

            dyn_string_add_string(label_string,"TYPE LF@type_var LF@param");
            unsigned_int_to_string(label_string,i);
            dyn_string_add_char(label_string,'\n');
            dyn_string_add_string(label_string,"JUMPIFEQ $ParCheckEnd");
            unsigned_int_to_string(label_string,i);
            if(node->declare_func.func_name->context != NULL)
            {
                dyn_string_add_string(label_string,node->declare_func.func_name->context->string);
            }
            dyn_string_add_string(label_string,node->declare_func.func_name->name->string);
            dyn_string_add_string(label_string," LF@type_var string@");

            switch (node->declare_func.func_name->info.function.arguments.TypesOfArguments[i])
            {
            case FLOAT_TYPE:
            case FLOAT_NULL_TYPE:
                dyn_string_add_string(label_string,"float\n");
                break;
            case INT_TYPE:
            case INT_NULL_TYPE:
                dyn_string_add_string(label_string,"int\n");
                break;
            case STRING_TYPE:
            case STRING_NULL_TYPE:
                dyn_string_add_string(label_string,"string\n");
                break;
            case NULL_TYPE:
                dyn_string_add_string(label_string,"nil\n");
                break;         
            default:
                dyn_string_add_string(label_string,"nil\n");
                break;
            }

            if(node->declare_func.func_name->info.function.arguments.TypesOfArguments[i] == INT_NULL_TYPE || node->declare_func.func_name->info.function.arguments.TypesOfArguments[i] == FLOAT_NULL_TYPE ||node->declare_func.func_name->info.function.arguments.TypesOfArguments[i] == STRING_NULL_TYPE)
            {
                dyn_string_add_string(label_string,"JUMPIFEQ $ParCheckEnd");
                unsigned_int_to_string(label_string,i);
                if(node->declare_func.func_name->context != NULL)
                {
                    dyn_string_add_string(label_string,node->declare_func.func_name->context->string);
                }
                dyn_string_add_string(label_string,node->declare_func.func_name->name->string);
                dyn_string_add_string(label_string," LF@type_var string@nil\n");
            }

            dyn_string_add_string(label_string,"EXIT int@4\n");
            dyn_string_add_string(label_string,"LABEL $ParCheckEnd");
            unsigned_int_to_string(label_string,i);
            if(node->declare_func.func_name->context != NULL)
            {
                dyn_string_add_string(label_string,node->declare_func.func_name->context->string);
            }
            dyn_string_add_string(label_string,node->declare_func.func_name->name->string);
            dyn_string_add_char(label_string,'\n');


        }
        dyn_string_add_string(label_string,"###################PARAM TYPE CONTROL END####################\n");

    }
    //tady def promene a dat do nich param hodnoty
    if(node->declare_func.func_name->info.function.arguments.countOfArguments > 0)
    {
        struct func_parameters *elem;
        elem = node->declare_func.arguments;
        Dyn_String *name = dyn_string_init();
        for(int i = 1; i <= node->declare_func.func_name->info.function.arguments.countOfArguments;i++) 
       {

            // DEFVAR LF@contextname
            dyn_string_add_string(name,node->declare_func.func_name->name->string);
            dyn_string_add_string(name,elem->op->name->string);
            dyn_string_add_string(label_string,"DEFVAR LF@");
            dyn_string_add_string(label_string,name->string);
            dyn_string_add_char(label_string,'\n'); 
            varListInsertLast(var_list,name);


            // MOVE LF@contextname
            dyn_string_add_string(label_string,"MOVE LF@");
            dyn_string_add_string(label_string,name->string);
            dyn_string_add_char(label_string,' ');


            //                    LF@parami
            dyn_string_add_string(label_string,"LF@param");
            unsigned_int_to_string(label_string,i);
            dyn_string_add_char(label_string,'\n'); 
            elem = elem ->next;
            dyn_string_clear(name);
       }
        dyn_string_free(name);
    }

    instListInsertLast(func_list,func_beg,label_string);


    
    //body
    //#####################################
    error_code = gen_from_ass(node->declare_func.func_body,func_list,func_list,if_count,var_list);
    {
        if(error_code != 0)
        {
            dyn_string_free(label_string);
            return error_code;
        }
    }
    //#####################################



    //end
    Dyn_String *end = dyn_string_init();
    //check for return type
    dyn_string_add_string(end,"#############CHECK RETURN TYPE###########\n");
    dyn_string_add_string(end,"DEFVAR LF@type_var_ret\n");
    dyn_string_add_string(end,"TYPE LF@type_var_ret LF@%retval1\n");
    dyn_string_add_string(end,"JUMPIFEQ $ReturnTypeCheck");
    dyn_string_add_string(end,node->declare_func.func_name->name->string);
    dyn_string_add_char(end,' ');
    dyn_string_add_string(end,"LF@type_var_ret string@");

    switch (node->declare_func.func_name->info.function.returnType)
    {
    case FLOAT_TYPE:
    case FLOAT_NULL_TYPE:
        dyn_string_add_string(end,"float\n");
        break;
    case INT_TYPE:
    case INT_NULL_TYPE:
        dyn_string_add_string(end,"int\n");
        break;
    case STRING_TYPE:
    case STRING_NULL_TYPE:
        dyn_string_add_string(end,"string\n");
        break;
    case NULL_TYPE:
        dyn_string_add_string(end,"nil\n");
        break;         
    default://justs to be safe
        dyn_string_add_string(end,"error\n");
        break;
    }
    if(node->declare_func.func_name->info.function.returnType == STRING_NULL_TYPE || node->declare_func.func_name->info.function.returnType == INT_NULL_TYPE || node->declare_func.func_name->info.function.returnType == FLOAT_NULL_TYPE)
    {
        dyn_string_add_string(end,"JUMPIFEQ $ReturnTypeCheck");
        dyn_string_add_string(end,node->declare_func.func_name->name->string);
        dyn_string_add_string(end," LF@type_var_ret string@nil\n");
    }

    dyn_string_add_string(end,"EXIT int@4\n");
    dyn_string_add_string(end,"LABEL $ReturnTypeCheck");
    dyn_string_add_string(end,node->declare_func.func_name->name->string);
    dyn_string_add_char(end,'\n');


    dyn_string_add_string(end,"POPFRAME\nRETURN\n");
    dyn_string_add_string(end,"##############################################################FUNC END######################################################\n");

    instListInsertLast(func_list,func_end,end);


    dyn_string_free(label_string);
    dyn_string_free(end);


    return 0;
}


//after calling function the return will be store in LF@%retval1
// melo by jit
int generate_call_func(node_t *node, inst_list_t *main_body_list)
{
    if(node->assigment_func.func->context == NULL && !strcmp(node->assigment_func.func->name->string,"write"))
    {
       return generate_call_func_write(node, main_body_list);
    }

    Dyn_String *call = dyn_string_init();
    dyn_string_add_string(call,"CREATEFRAME\n");

    func_par_t* par = node->assigment_func.parameters;

    for(int i = 1; i <= node->assigment_func.func->info.function.arguments.countOfArguments;i++) 
    {
        dyn_string_add_string(call,"DEFVAR TF@%");
        unsigned_int_to_string(call,i);
        dyn_string_add_char(call,'\n');
        dyn_string_add_string(call,"MOVE TF@%");
        unsigned_int_to_string(call,i);
        dyn_string_add_char(call,' ');
        //tedka bud imm nebo jinou var
        //jako argumenty
        // switch (node->assigment_func.func->info.function.arguments.TypesOfArguments[i-1])
        switch (par->type)
        {
            case par_op:
                // if(node->assigment_func.parameters[i-1].op->context == NULL)

                if(par->op->context == NULL)
                {
                    dyn_string_add_string(call,"GF@");
                    if(par->op->context != NULL)
                    {
                        dyn_string_add_string(call,par->op->context->string);
                    }
                    dyn_string_add_string(call,par->op->name->string);
                    dyn_string_add_char(call,'\n');
                }
                else
                {
                    dyn_string_add_string(call,"LF@");
                    if(par->op->context != NULL)
                    {
                        dyn_string_add_string(call,par->op->context->string);
                    }
                    dyn_string_add_string(call,par->op->name->string);
                    dyn_string_add_char(call,'\n');
                }

                break;
            case par_imm:
                switch (par->imm.type)
                {
                    case type_int:
                        dyn_string_add_string(call,"int@");
                        unsigned_int_to_string(call,par->imm.data.type_int);
                        dyn_string_add_char(call,'\n');
                        break;
                    case type_float:
                        dyn_string_add_string(call,"float@");
                        char float_string[100];
                        sprintf(float_string,"%a",par->imm.data.type_float);
                        dyn_string_add_string(call,float_string);
                        dyn_string_add_char(call,'\n');
                        break;
                    case type_string:
                        dyn_string_add_string(call,"string@");
                        dyn_string_add_string(call,par->imm.data.type_string->string);
                        dyn_string_add_char(call,'\n');
                        break;
                }

                break;       
            case par_null:
                dyn_string_add_string(call,"nil@nil\n");
                break;       
            default:

                dyn_string_free(call);
                return 1;
                break;
        }
        par = par->next;


    }
    

    //CALL LABEL
    dyn_string_add_string(call,"CALL $");
    if(node->assigment_func.func->context != NULL)
    {
        dyn_string_add_string(call,node->assigment_func.func->context->string);
    }
    dyn_string_add_string(call,node->assigment_func.func->name->string);
    dyn_string_add_char(call,'\n');
    instListInsertLast(main_body_list,func_call,call);
    dyn_string_free(call);
    return 0;
}


int generate_call_func_write(node_t *node, inst_list_t *main_body_list)
{

    Dyn_String *call = dyn_string_init();
    func_par_t* par = node->assigment_func.parameters;
    // for(int i = 1; i <= node->assigment_func.func->info.function.arguments.countOfArguments;i++) 
    for(int i = 1; par != NULL;i++) 
    {
        dyn_string_add_string(call,"CREATEFRAME\n");
        dyn_string_add_string(call,"DEFVAR TF@%1");
        // unsigned_int_to_string(call,i);
        dyn_string_add_char(call,'\n');
        dyn_string_add_string(call,"MOVE TF@%1");
        // unsigned_int_to_string(call,i);
        dyn_string_add_char(call,' ');

        //tedka bud imm nebo jinou var
        //jako argumenty
        switch (par->type)
        {
            case par_op:
                if(par->op->context == NULL)
                {
                    dyn_string_add_string(call,"GF@");
                    dyn_string_add_string(call,par->op->name->string);
                    dyn_string_add_char(call,'\n');
                }
                else
                {
                    dyn_string_add_string(call,"LF@");
                    dyn_string_add_string(call,par->op->context->string);
                    dyn_string_add_string(call,par->op->name->string);
                    dyn_string_add_char(call,'\n');
                }
                break;
            case par_imm:
                switch (par->imm.type)
                {
                    case type_int:
                        dyn_string_add_string(call,"int@");
                        unsigned_int_to_string(call,par->imm.data.type_int);
                        dyn_string_add_char(call,'\n');
                        break;
                    case type_float:
                        dyn_string_add_string(call,"float@");
                        char float_string[100];
                        sprintf(float_string,"%a",par->imm.data.type_float);
                        dyn_string_add_string(call,float_string);
                        dyn_string_add_char(call,'\n');
                        break;
                    case type_string:
                        dyn_string_add_string(call,"string@");
                        dyn_string_add_string(call,par->imm.data.type_string->string);
                        dyn_string_add_char(call,'\n');
                        break;
                }
                break;       
            case par_null:
                dyn_string_add_string(call,"nil@nil\n");
                break;       
            default:
                dyn_string_free(call);
                return 1;
                break;
        }
        //CALL LABEL
        dyn_string_add_string(call,"CALL $");
        if(node->assigment_func.func->context != NULL)
        {
            dyn_string_add_string(call,node->assigment_func.func->context->string);
        }
        dyn_string_add_string(call,node->assigment_func.func->name->string);
        dyn_string_add_char(call,'\n');
        instListInsertLast(main_body_list,func_call,call);
        dyn_string_clear(call);
        par = par ->next;

    }
    dyn_string_free(call);
    return 0;
}


// melo by fungovat
int generate_func_asign(node_t *node, inst_list_t *main_body_list,var_generated_t *var_list)
{    
    int error_code =  generate_call_func(node,main_body_list);
    //fce call uz je v listu

    if(node->assigment_func.target==NULL)
    {
        return 0;
    }

    Dyn_String *var_name = dyn_string_init();
    if(node->assigment_func.target->context != NULL)
    {
        dyn_string_add_string(var_name,node->assigment_func.target->context->string);
    }
    dyn_string_add_string(var_name,node->assigment_func.target->name->string);

    Dyn_String *assign = dyn_string_init();


    // pokud nebyla def
    if(!varListFind(var_list,var_name))
    {
        //pridam name do var listu
        //var dame typ return fce
        // varListInsertLast(var_list,var_name,node->assigment_func.func->info.function.returnType);
        varListInsertLast(var_list,var_name);

        //kdyz je v mainu tak var musi byt gf asi
        if(node->assigment_func.target->context == NULL)
        {
            dyn_string_add_string(assign,"DEFVAR GF@");
        }
        else // ve fci
        {
            dyn_string_add_string(assign,"DEFVAR LF@");
        }
        dyn_string_add_string(assign,var_name->string);
        dyn_string_add_char(assign,'\n');
        //kdyby to byl while
        instInsertBeforeWhile(main_body_list,var_dec,assign);

        dyn_string_clear(assign);
    }

    if(node->assigment_func.target->context == NULL)
    {
        dyn_string_add_string(assign,"MOVE GF@");
    }
    else
    {
        dyn_string_add_string(assign,"MOVE LF@");
    }
    dyn_string_add_string(assign,var_name->string);
    dyn_string_add_char(assign,' ');
    dyn_string_add_string(assign,"TF@%retval1\n");
    dyn_string_free(var_name);



    instListInsertLast(main_body_list,func_assign,assign);
    dyn_string_free(assign);


    return error_code;


}

// mela by fungovat
//vysledek na stacku
int ev_expression(node_t *node, inst_list_t *main_body_list)
{
    Dyn_String *express = dyn_string_init();
    // printf("!!%d",node->expression.operator);
    switch (node->expression.operator)
    {
    //pouze u jednoducheho prirazeni jako $x=5 nebo $x=$y
    case SingleOp:
                if(node->expression.left->type == op)
                {
                    dyn_string_add_string(express,"PUSHS ");
                    if(node->expression.left->data.op->context == NULL)
                    {
                        dyn_string_add_string(express,"GF@");
                    }
                    else
                    {
                        dyn_string_add_string(express,"LF@");
                    }
                    if(node->expression.left->data.op->context != NULL)
                    {
                        dyn_string_add_string(express,node->expression.left->data.op->context->string);
                    }
                    dyn_string_add_string(express,node->expression.left->data.op->name->string);
                    dyn_string_add_char(express,'\n');
                    instListInsertLast(main_body_list,expression_ev,express);
                    dyn_string_free(express);
                    
                    return 0;
                }
                else if(node->expression.left->type == imm)
                {

                    switch (node->expression.left->data.imm.type)
                    {
                    case type_int:
                        dyn_string_add_string(express,"PUSHS int@");
                        unsigned_int_to_string(express,node->expression.left->data.imm.data.type_int);
                        dyn_string_add_char(express,'\n');
                        break;
                    case type_float:
                        dyn_string_add_string(express,"PUSHS float@");
                        char float_string[100];
                        sprintf(float_string,"%a",node->expression.left->data.imm.data.type_float);
                        dyn_string_add_string(express,float_string);
                        dyn_string_add_char(express,'\n');
                        break;
                    case type_string:
                        dyn_string_add_string(express,"PUSHS string@");
                        dyn_string_add_string(express,node->expression.left->data.imm.data.type_string->string);
                        dyn_string_add_char(express,'\n');
                        break;        
                    default:
                        dyn_string_free(express);
                        return 1;
                        break;
                    }
                    instListInsertLast(main_body_list,expression_ev,express);
                    dyn_string_free(express);
                    return 0;
                }
                // jinak error

                dyn_string_free(express);
                return 1;

    case Plus:
    case Minus:
    case Multiply:
    case Dot:
    case Divide:
    case Equal:
    case NotEqual:
    case Greater:
    case Lesser:
    case GreaterEqual:
    case LesserEqual:
        

                //ted se postarat o pravou vetev
        //pokud sme tady tak v provo je bud hodnota ne bo dalsi expr (ne single)
        //pak ty operace
        switch (node->expression.right->type)
        {
        case op:
            dyn_string_add_string(express,"PUSHS ");
            if(node->expression.right->data.op->context == NULL)
            {
                dyn_string_add_string(express,"GF@");
            }
            else
            {
                dyn_string_add_string(express,"LF@");
            }
            if(node->expression.right->data.op->context != NULL)
            {
                dyn_string_add_string(express,node->expression.right->data.op->context->string);
            }
            dyn_string_add_string(express,node->expression.right->data.op->name->string);
            dyn_string_add_char(express,'\n');
            break;
        case imm:
                switch (node->expression.right->data.imm.type)
                {
                case type_int:
                    dyn_string_add_string(express,"PUSHS int@");
                    unsigned_int_to_string(express,node->expression.right->data.imm.data.type_int);
                    dyn_string_add_char(express,'\n');
                    break;
                case type_float:
                    dyn_string_add_string(express,"PUSHS float@");
                    char float_string[100];
                    sprintf(float_string,"%a",node->expression.right->data.imm.data.type_float);
                    dyn_string_add_string(express,float_string);
                    dyn_string_add_char(express,'\n');
                    break;
                case type_string:
                    dyn_string_add_string(express,"PUSHS string@");
                    dyn_string_add_string(express,node->expression.right->data.imm.data.type_string->string);
                    dyn_string_add_char(express,'\n');
                    break;        
                default:
                    break;
                }
            break;
        case exp:
            ev_expression(&(node->expression.right->data.exp->thiscommand), main_body_list); 
            break;
        case nul:
            dyn_string_add_string(express,"PUSHS nil@nil\n");
            break;

          default:
            return 1;
            break;
        }
        instListInsertLast(main_body_list,expression_ev,express);
        dyn_string_clear(express);

        // vlevo je op, bude push
        if(node->expression.left->type == op)
        {
            dyn_string_add_string(express,"PUSHS ");
            if(node->expression.left->data.op->context == NULL)
            {
                dyn_string_add_string(express,"GF@");
            }
            else
            {
                dyn_string_add_string(express,"LF@");
            }
            if(node->expression.left->data.op->context != NULL)
            {
                dyn_string_add_string(express,node->expression.left->data.op->context->string);
            }
            dyn_string_add_string(express,node->expression.left->data.op->name->string);
            dyn_string_add_char(express,'\n');
        }
        //vlevo je imm,bude push
        else if(node->expression.left->type == imm)
        {
            switch (node->expression.left->data.imm.type)
                {
                case type_int:
                    dyn_string_add_string(express,"PUSHS int@");
                    unsigned_int_to_string(express,node->expression.left->data.imm.data.type_int);
                    dyn_string_add_char(express,'\n');
                    break;
                case type_float:
                    dyn_string_add_string(express,"PUSHS float@");
                    char float_string[100];
                    sprintf(float_string,"%a",node->expression.left->data.imm.data.type_float);
                    dyn_string_add_string(express,float_string);
                    dyn_string_add_char(express,'\n');
                    break;
                case type_string:
                    dyn_string_add_string(express,"PUSHS string@");
                    dyn_string_add_string(express,node->expression.left->data.imm.data.type_string->string);
                    dyn_string_add_char(express,'\n');
                    break;        
                default:
                    break;
                }
        }
        else if(node->expression.left->type == exp)
        {
            ev_expression(&(node->expression.left->data.exp->thiscommand), main_body_list); 
        }
        else if(node->expression.left->type == nul)
        {
            dyn_string_add_string(express,"PUSHS nil@nil\n");

        }




    default:
        break;
    }
    //ted jen provist operace nad 2mi vetvemi, obe jsou na zasobniku
    //vysledek na zasobnik
    //neni singleOP

    //jaj
    switch (node->expression.operator)
    {
    case Plus:
        dyn_string_add_string(express,"CALL $&convert_for_math\n");
        dyn_string_add_string(express,"ADDS\n");
        break;
    case Minus:
        dyn_string_add_string(express,"CALL $&convert_for_math\n");
        dyn_string_add_string(express,"SUBS\n");
        break;
    case Multiply:
        dyn_string_add_string(express,"CALL $&convert_for_math\n");
        dyn_string_add_string(express,"MULS\n");
        break;
    case Divide:
        dyn_string_add_string(express,"CALL $&convert_for_div\n");
        dyn_string_add_string(express,"DIVS\n");
        break;
    case Dot:
        dyn_string_add_string(express,"CALL $&convert_for_conc\n");
        dyn_string_add_string(express,"CALL $&conc\n");
        break;
    case Equal:
        dyn_string_add_string(express,"CALL $&equal_type\n");
        break;
    case NotEqual:
        dyn_string_add_string(express,"CALL $&not_equal_type\n");
        break;
    case Greater:
        dyn_string_add_string(express,"CALL $&convert_for_gl\n");
        dyn_string_add_string(express,"GTS\n");
        break;
    case Lesser:
        dyn_string_add_string(express,"CALL $&convert_for_gl\n");
        dyn_string_add_string(express,"LTS\n");
        break;
    case GreaterEqual:
        dyn_string_add_string(express,"CALL $&convert_for_gl_equal\n");
        dyn_string_add_string(express,"CALL $&greater_equal\n");
        break;
    case LesserEqual:
        dyn_string_add_string(express,"CALL $&convert_for_gl_equal\n");
        dyn_string_add_string(express,"CALL $&lesser_equal\n");
        break;
    case Error:
        return 1;
        break;
    default:
        break;
    }
    instListInsertLast(main_body_list,expression_ev,express);
    dyn_string_free(express);
    return 0;   
}

// mela by fungovat
int gen_assig_expression(node_t *node, inst_list_t *main_body_list,var_generated_t *var_list)
{
    
    int error_code =  ev_expression(&(node->assigment_expression.expression->thiscommand),main_body_list);



    Dyn_String *var_name = dyn_string_init();
    if(node->assigment_expression.target->context != NULL)
    {
        dyn_string_add_string(var_name,node->assigment_expression.target->context->string);
    }
    dyn_string_add_string(var_name,node->assigment_expression.target->name->string);

    Dyn_String *assign = dyn_string_init();


    // pokud nebyla def
    if(!varListFind(var_list,var_name))
    {
        //pridam name do var listu
        varListInsertLast(var_list,var_name);

        //kdyz je v mainu tak var musi byt gf asi
        if(node->assigment_expression.target->context == NULL)
        {
            dyn_string_add_string(assign,"DEFVAR GF@");
        }
        else // ve fci
        {
            dyn_string_add_string(assign,"DEFVAR LF@");
        }
        dyn_string_add_string(assign,var_name->string);
        dyn_string_add_char(assign,'\n');
        instInsertBeforeWhile(main_body_list,var_dec,assign);
        dyn_string_clear(assign);
    }

    dyn_string_add_string(assign,"POPS ");
    if(node->assigment_expression.target->context == NULL)
    {
        dyn_string_add_string(assign,"GF@");
    }
    else
    {
        dyn_string_add_string(assign,"LF@");
    }
    dyn_string_add_string(assign,var_name->string);
    dyn_string_add_char(assign,'\n');

    instListInsertLast(main_body_list,var_def,assign);
    dyn_string_free(var_name);
    dyn_string_free(assign);
    return error_code;
    
}


//melo by fungovat
int generate_if(node_t *node, inst_list_t *main_body_list,int *if_count,inst_list_t *func_list,var_generated_t *var_list)
{
    int error_code;
    *if_count = *if_count + 1;

    Dyn_String *if_code = dyn_string_init();
    Dyn_String *label_base_name = dyn_string_init();
    dyn_string_add_string(label_base_name,"$if");
    unsigned_int_to_string(label_base_name,*if_count);

    error_code = ev_expression(&(node->if_statement.expression->thiscommand),main_body_list);
    if(error_code != 0)
    {
        dyn_string_free(if_code);
        dyn_string_free(label_base_name);
        return error_code;
    }

    //na stacku mam vyhodnoceni
    dyn_string_add_string(if_code,"CALL $&ev_if_to_ret\n");
    //v LF@%retval1 je bool
    dyn_string_add_string(if_code,"JUMPIFEQ ");
    dyn_string_add_string(if_code,label_base_name->string);
    dyn_string_add_string(if_code,"true ");
    dyn_string_add_string(if_code,"TF@%retval1 bool@true\n");
    instListInsertLast(main_body_list,if_cond,if_code);
    dyn_string_clear(if_code);
    //false vetev
    //
    error_code = gen_from_ass(node->if_statement.false_block,main_body_list,func_list,if_count,var_list);
    if(error_code)
    {
        dyn_string_free(if_code);
        dyn_string_free(label_base_name);

        return error_code;
    }

    //JUMP uplny end
    dyn_string_add_string(if_code,"JUMP ");
    dyn_string_add_string(if_code,label_base_name->string);
    dyn_string_add_string(if_code,"end\n");

    //LABEL true vetev
    dyn_string_add_string(if_code,"LABEL ");
    dyn_string_add_string(if_code,label_base_name->string);
    dyn_string_add_string(if_code,"true\n");
    instListInsertLast(main_body_list,if_false,if_code);
    dyn_string_clear(if_code);
    error_code = gen_from_ass(node->if_statement.true_block,main_body_list,func_list,if_count,var_list);
    if(error_code)
    {
        dyn_string_free(if_code);
        dyn_string_free(label_base_name);
        return error_code;
    }

    //LABEL uplny end
    dyn_string_add_string(if_code,"LABEL ");
    dyn_string_add_string(if_code,label_base_name->string);
    dyn_string_add_string(if_code,"end\n");

    instListInsertLast(main_body_list,if_end,if_code);
    dyn_string_free(if_code);
    dyn_string_free(label_base_name);
    return 0;

}

//melo by fungovat
int generate_while(node_t *node, inst_list_t *main_body_list, inst_list_t *func_list,var_generated_t *var_list,int *if_count)
{

    int error_code;
    *if_count = *if_count + 1;
    Dyn_String *while_code = dyn_string_init();
    Dyn_String *label_base_name = dyn_string_init();
    dyn_string_add_string(label_base_name,"$while");
    unsigned_int_to_string(label_base_name,*if_count);

    //zarazka pro while
    Dyn_String *empty = dyn_string_init();
    instListInsertLast(main_body_list,while_loop_start,empty);
    //LABEL WHILExstart
    dyn_string_add_string(while_code,"LABEL ");
    dyn_string_add_string(while_code,label_base_name->string);
    dyn_string_add_string(while_code,"start\n");
    instListInsertLast(main_body_list,label_def,while_code);
    dyn_string_clear(while_code);

    // instListInsertLast
    //JUMPIFNEQ ZA ev true
    error_code = ev_expression(&(node->while_statement.expression->thiscommand),main_body_list);
    {
        if(error_code != 0)
        {
            dyn_string_free(while_code);
            dyn_string_free(label_base_name);
            dyn_string_free(empty);
            return error_code;
        }
    }
    //vysl v retval
    dyn_string_add_string(while_code,"CALL $&ev_if_to_ret\n");

    dyn_string_add_string(while_code,"JUMPIFNEQ ");
    dyn_string_add_string(while_code,label_base_name->string);
    dyn_string_add_string(while_code,"end ");
    dyn_string_add_string(while_code,"TF@%retval1 bool@true\n");
    instListInsertLast(main_body_list,label_def,while_code);
    dyn_string_clear(while_code);


    //BODY
    //
    gen_from_ass(node->while_statement.while_block,main_body_list,func_list,if_count,var_list);
    //////

    //JUMP WHILE START
    dyn_string_add_string(while_code,"JUMP ");
    dyn_string_add_string(while_code,label_base_name->string);
    dyn_string_add_string(while_code,"start\n");
    //LABEL whileXend
    dyn_string_add_string(while_code,"LABEL ");
    dyn_string_add_string(while_code,label_base_name->string);
    dyn_string_add_string(while_code,"end\n");

    instListInsertLast(main_body_list,while_loop_body,while_code);
    //zarazka konec while
    instListInsertLast(main_body_list,while_loop_end,empty);

    dyn_string_free(empty);
    dyn_string_free(while_code);
    dyn_string_free(label_base_name);
    return 0;
}

int generate_return(node_t *node, inst_list_t *main_body_list)
{
    if(node->return_statement.expression != NULL)
    {
        Dyn_String *return_string = dyn_string_init();
        dyn_string_add_string(return_string,"####RETERN START######\n");
        int error_code =  ev_expression(&(node->return_statement.expression->thiscommand),main_body_list);
        dyn_string_add_string(return_string,"POPS LF@%retval1\n");
        dyn_string_add_char(return_string,'\n');
        instListInsertLast(main_body_list,return_def,return_string);
        dyn_string_free(return_string);
        return error_code;
    }
    else
    {   //dam jen return a nehrotim
        Dyn_String *return_string = dyn_string_init();
        dyn_string_add_string(return_string,"####RETERN START######\n");
        dyn_string_add_string(return_string,"RETURN\n");
        instListInsertLast(main_body_list,return_def,return_string);
        dyn_string_free(return_string);
    }
    return 0;
    
}
