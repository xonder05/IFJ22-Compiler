#include "scanner.h"
#include "code_gen_build.h"
#include "dyn_string.h"
#include "abstact_syntax_tree.h"

//parametry strom, main_list,func_list
//returns 0 on succes, error code on failure
int gen_from_ass(){
    return 0;
}

//nejakej strom parametr
//returns 0 on succes, error code on failure
int generate()
{
    int error_code = 0;

    //create main dll
    inst_list_t main_list,main_body_list,func_list;
    instListInit(&main_list);
    instListInit(&main_body_list);
    instListInit(&func_list);

    //gen built-in
    def_built_in(&func_list);


    //go through tree, generate etc...
    error_code = gen_from_ass();
    if(error_code != 0)
    {
        return error_code;
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


    printList(&main_list);
    isntListDispose(&main_list);

    return 0;
}

int generate_func(node_t *node, inst_list_t *func_list)
{  

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
    dyn_string_add_string(label_string,node->declare_func.func_name->context->string);
    dyn_string_add_string(label_string,node->declare_func.func_name->name->string);
    dyn_string_add_char(label_string,'\n'); 
    dyn_string_add_string(label_string,"PUSHFRAME\n");

    if(node->declare_func.func_name->info.function.arguments.countOfArguments > 0)
    {

       for(int i = 1; i <= node->declare_func.func_name->info.function.arguments.countOfArguments;i++) 
       {
            // DEFVAR LF@param1
            dyn_string_add_string(label_string,"DEFVAR LF@param");
            unsigned_int_to_string(label_string,i);
            dyn_string_add_char(label_string,'\n'); 

            // MOVE LF@param1 LF@%1
            dyn_string_add_string(label_string,"MOVE LF@param1 LF@%");
            unsigned_int_to_string(label_string,i);
            dyn_string_add_char(label_string,'\n'); 

       }
    }

    instListInsertLast(func_list,func_beg,label_string);

    //TODO###################################
    //check arguments type and actual params
    //middle part
    //generate func body
    //TODO###################################


    //end
    Dyn_String *end = dyn_string_init();
    dyn_string_add_string(end,"POPFRAME\nRETURN\n");
    dyn_string_add_string(end,"##############################################################FUNC END######################################################\n");

    instListInsertLast(func_list,func_end,end);


    dyn_string_free(label_string);
    dyn_string_free(end);


    return 0;
}


