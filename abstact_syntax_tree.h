#pragma once 
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"

typedef union node node_t;
typedef struct abstactSyntaxTree ast_t;


//immediate operands are stored directly in the syntax tree
typedef enum {type_int, type_float, type_string}imm_type_t;
typedef union imm_uni {
        long int type_int;
        double type_float;
        Dyn_String *type_string;
}imm_uni_t;
typedef struct immediate_operand{
    imm_type_t type;
    imm_uni_t data;
}imm_t;


// subtree of an expression can consist of multiple different data types
typedef struct expression_subtree{
    enum sub_tree_type {op, exp, imm, nul}type;
    union exp_sub_uni{
        symbol_t *op; 
        ast_t *exp;
        imm_t imm;
    }data;
}exp_subtree_t;

// expression operators
typedef enum {SingleOp, Plus, Minus, Multiply, Divide, Dot, Equal, NotEqual, Greater, Lesser, GreaterEqual, LesserEqual, Error = -1
}operator;

// list-like structure for storing funtion parameters
typedef struct func_parameters{
    enum {first, par_op, par_imm, par_null}type;
    symbol_t *op;
    imm_t imm;
    struct func_parameters *next;
}func_par_t;


//node data based on it's type
union node{
    bool prologSuccess;
    struct {symbol_t* target; ast_t *expression;} assigment_expression;
    struct {symbol_t* target; symbol_t* func; func_par_t* parameters;} assigment_func;
    struct {symbol_t* func_name; ast_t *func_body;} declare_func;
    struct {ast_t *expression; ast_t *true_block; ast_t *false_block;} if_statement;
    struct {ast_t *expression; ast_t *while_block; } while_statement;
    struct {operator operator; exp_subtree_t *left; exp_subtree_t *right;} expression;
    struct {ast_t *expression;} return_statement;
};

//node type in abstract syntax tree
typedef enum{ root, assigment_expression, assigment_func, declare_func, if_statement, while_statement, expression, return_statement
}nodeType;

struct abstactSyntaxTree{
    nodeType type;
    node_t thiscommand;
    struct abstactSyntaxTree* nextcommand;
};

//node creation
ast_t* createRootNode(bool prologSuccess);
ast_t* createAssigmentExpNode(symbol_t* target, ast_t* expression);
ast_t* createAssigmentFuncNode(symbol_t* target, symbol_t* func);
ast_t* createDeclareFuncNode(symbol_t* func_name, ast_t *func_body);
ast_t* createIfNode(ast_t *expression, ast_t *true_block, ast_t *false_block);
ast_t* createWhileNode(ast_t *expression, ast_t *while_block);
ast_t* createExpressionNode(operator operator, exp_subtree_t *left, exp_subtree_t *right);  
ast_t* createReturnNode(ast_t *expression);

//dispose entire tree
void disposeTree(ast_t* tree);

//ugly, for testing purposes
ast_t* printTree(ast_t *tree);



// expression subtree operations
exp_subtree_t* createExpSubtree(symbol_t* symbol, ast_t* subtree, long int* imm_int, double* imm_float, Dyn_String* imm_string);
void diposeExpSubtree(exp_subtree_t* tree);


// operation with function parameters list
func_par_t* parInit();
func_par_t* addParametrer(func_par_t* parameters, symbol_t* symbol, long int* int_input, double* float_input, Dyn_String* string_input);
void disposeParameters (func_par_t* parameters);
