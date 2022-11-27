#pragma once 
#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"

typedef union node node_t;
typedef struct abstactSyntaxTree ast_t;

typedef enum{
    root, assigment_expression, assigment_func, declare_func, if_statement, while_statement, expression
}nodeType;

typedef enum {plus, minus, multiply, divide, equal, notEqual, greater, lesser, greaterEqual, lesserEqual
}operator;

//todo operations with this
typedef struct expression_subtree{
    enum {op, exp}type;
    union exp_sub_uni{
        symbol_t* op; 
        ast_t *expression;
    }data;
}exp_subtree;

//node data based on it's type
union node{
    bool prologSuccess;
    struct {symbol_t* target; ast_t *expression;} assigment_expression;
    struct {symbol_t* target; symbol_t* func;} assigment_func;
    struct {symbol_t* func_name; ast_t *func_body;} declare_func;
    struct {ast_t *expression; ast_t *true_block; ast_t *false_block;} if_statement;
    struct {ast_t *expression; ast_t *while_block; } while_statement;
    struct {operator operator; exp_subtree *left; exp_subtree *right;} expression;
};

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
ast_t* createExpressionNode(operator operator, exp_subtree *left, exp_subtree *right);  

//dispose entire tree
void disposeTree(ast_t* tree);

//ugly, for testing purposes
ast_t* printTree(ast_t *tree);