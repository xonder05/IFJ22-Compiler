#include "abstact_syntax_tree.h"

ast_t* createRootNode(bool prologSuccess)
{
    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "malloc fail\n"); 
        return NULL;
    }

    strom->type = root;
    strom->nextcommand = NULL;
    strom->thiscommand.prologSuccess = prologSuccess;
    return strom;
}

ast_t* createAssigmentExpNode(symbol_t* target, ast_t* expression)
{
    /*
    if (target == NULL || expression == NULL)
    {
        return NULL;
    }
    */

    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "mallock fail\n"); 
        return NULL;
    }

    strom->type = assigment_expression;
    strom->nextcommand = NULL;
    strom->thiscommand.assigment_expression.target = target;
    strom->thiscommand.assigment_expression.expression = expression;
    return strom;
}

ast_t* createAssigmentFuncNode(symbol_t* target, symbol_t* func)
{
    /*
    if (target == NULL || func == NULL)
    {
        return NULL;
    }
    */

    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "mallock fail\n"); 
        return NULL;
    }

    strom->type = assigment_func;
    strom->nextcommand = NULL;
    strom->thiscommand.assigment_func.target = target;
    strom->thiscommand.assigment_func.func = func;
    return strom;
}

ast_t* createDeclareFuncNode(symbol_t* func_name, ast_t *func_body)
{
    /*
    if (func_name == NULL || func_body == NULL)
    {
        return NULL;
    }
    */

    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "mallock fail\n"); 
        return NULL;
    }

    strom->type = declare_func;
    strom->nextcommand = NULL;
    strom->thiscommand.declare_func.func_name = func_name;
    strom->thiscommand.declare_func.func_body = func_body;
    return strom;
}

ast_t* createIfNode(ast_t *expression, ast_t *true_block, ast_t *false_block)
{
    /*
    if (expression == NULL || true_block == NULL || false_block == NULL)
    {
        return NULL;
    }
    */

    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "mallock fail\n"); 
        return NULL;
    }

    strom->type = if_statement;
    strom->nextcommand = NULL;
    strom->thiscommand.if_statement.expression = expression;
    strom->thiscommand.if_statement.true_block = true_block;
    strom->thiscommand.if_statement.false_block = false_block;
    return strom;
}

ast_t* createWhileNode(ast_t *expression, ast_t *while_block)
{
    /*
    if (expression == NULL || while_block == NULL)
    {
        return NULL;
    }
    */

    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "mallock fail\n"); 
        return NULL;
    }

    strom->type = while_statement;
    strom->nextcommand = NULL;
    strom->thiscommand.while_statement.expression = expression;
    strom->thiscommand.while_statement.while_block = while_block;
    return strom;
}

ast_t* createExpressionNode(operator operator, exp_subtree_t *left, exp_subtree_t *right)
{
    /*
    if (left == NULL || (operator != SingleOp && right == NULL))
    {
        return NULL;
    }
    */

    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "mallock fail\n"); 
        return NULL;
    }

    strom->type = expression;
    strom->nextcommand = NULL;
    strom->thiscommand.expression.operator = operator;
    strom->thiscommand.expression.left = left;
    strom->thiscommand.expression.right = right;
    return strom;
}

ast_t* createReturnNode(ast_t *expression)
{
    /*
    if (expression == NULL)
    {
        return NULL;
    }
    */

    ast_t *strom = malloc(sizeof(struct abstactSyntaxTree));
    if (strom == NULL) 
    { 
        fprintf(stderr, "mallock fail\n"); 
        return NULL;
    }

    strom->type = return_statement;
    strom->nextcommand = NULL;
    strom->thiscommand.return_statement.expression = expression;
    return strom;
}

void disposeTree(ast_t* tree)
{
    if (tree == NULL)
    {
        return;
    }

    switch (tree->type)
    {
    case root:
        if (tree->nextcommand != NULL)
        {
            disposeTree(tree->nextcommand);
        }
        free(tree);
        break;

    case assigment_expression:
        if (tree->thiscommand.assigment_expression.expression != NULL)
        {
            disposeTree(tree->thiscommand.assigment_expression.expression);
        }
        if (tree->nextcommand != NULL)
        {
            disposeTree(tree->nextcommand);
        }
        free(tree);
        break;
    
    case assigment_func:
        if (tree->nextcommand != NULL)
        {
            disposeTree(tree->nextcommand);
        }
        free(tree);
        break;
    
    case declare_func:
        if (tree->thiscommand.declare_func.func_body != NULL)
        {
            disposeTree(tree->thiscommand.declare_func.func_body);
        }
        if (tree->nextcommand != NULL)
        {
            disposeTree(tree->nextcommand);
        }
        free(tree);
        break;

    case if_statement:
        if(tree->thiscommand.if_statement.expression != NULL)
        {
            disposeTree(tree->thiscommand.if_statement.expression);
        }
        if(tree->thiscommand.if_statement.true_block != NULL)
        {
            disposeTree(tree->thiscommand.if_statement.true_block);
        }
        if(tree->thiscommand.if_statement.false_block != NULL)
        {
            disposeTree(tree->thiscommand.if_statement.false_block);
        }
        if(tree->nextcommand != NULL)
        {
            disposeTree(tree->nextcommand);
        }
        free(tree);
        break;

    case while_statement:
        if (tree->thiscommand.while_statement.expression != NULL)
        {
            disposeTree(tree->thiscommand.while_statement.expression);
        }
        if (tree->thiscommand.while_statement.while_block != NULL)
        {
            disposeTree(tree->thiscommand.while_statement.expression);
        }
        if (tree->nextcommand != NULL)
        {
            disposeTree(tree->nextcommand);
        }
        free(tree);
        break;

    case expression:
        if(tree->thiscommand.expression.left != NULL)
        {
            diposeExpSubtree(tree->thiscommand.expression.left);
        }
        if(tree->thiscommand.expression.right != NULL)
        {
            diposeExpSubtree(tree->thiscommand.expression.right);
        }
        if (tree->nextcommand != NULL)
        {
            disposeTree(tree->nextcommand);
        }
        free(tree);
        break;
    default:
        break;
    }
}

void printTree(ast_t *tree)
{
    switch (tree->type)
    {
    case root:
        printf("Root Node\n");
        break;
    case assigment_expression:
        printf("Node type = AssigmentExp\n");
        printf("Target variable name = %s\n", tree->thiscommand.assigment_expression.target->name->string);
        printTree(tree->thiscommand.assigment_expression.expression);
        break;
    
    case assigment_func:
        printf("Node type = AssigmentFunc\n");
        printf("Target variable name = %s\n", tree->thiscommand.assigment_func.target->name->string);
        printf("Function name = %s\n", tree->thiscommand.assigment_func.func->name->string);
        break;
    
    case declare_func:
        printf("Node type = Declare Func\n");
        printf("Func name = %s\n", tree->thiscommand.declare_func.func_name->name->string);
        printf("Commands of func body:\n");
        printTree(tree->thiscommand.declare_func.func_body);
        printf("End of func body\n");
        break;

    case if_statement:
        printf("Node type = IF\n");
        printTree(tree->thiscommand.if_statement.expression);
        printf("True Block:\n");
        printTree(tree->thiscommand.if_statement.true_block);
        printf("False Block:\n");
        printTree(tree->thiscommand.if_statement.false_block);
        printf("End of if bodies\n");
        break;

    case while_statement:
        printf("Node type = While\n");
        printTree(tree->thiscommand.while_statement.expression);
        printf("Commands of while body:\n");
        printTree(tree->thiscommand.while_statement.while_block);
        printf("End of while body\n");
        break;

    case return_statement:
        printf("Node type = Return\n");
        printTree(tree->thiscommand.return_statement.expression);
        break;

    case expression:
        printf("Expression\n");
        printf("Operator = %d\n", tree->thiscommand.expression.operator);

        bool gotoLeftSubtree = false;
        bool gotoRightSubtree = false;

        printf("Left Operand = ");
        if (tree->thiscommand.expression.left->type == exp)
        {
            printf("tree\n");
            gotoLeftSubtree = true; 
        }
        else if (tree->thiscommand.expression.left->type == op)
        {
            printf("Variable %s\n", tree->thiscommand.expression.left->data.op->name->string);
        }
        else if (tree->thiscommand.expression.left->type == imm)
        {
            if (tree->thiscommand.expression.left->data.imm.type == type_int)
            {
                printf("Imm int %ld \n", tree->thiscommand.expression.left->data.imm.data.type_int);
            }
            else if (tree->thiscommand.expression.left->data.imm.type == type_float)
            {
                 printf("Imm float %f \n", tree->thiscommand.expression.left->data.imm.data.type_float);
            }
            else if (tree->thiscommand.expression.left->data.imm.type == type_string)
            {
                printf("Imm string %s \n", tree->thiscommand.expression.left->data.imm.data.type_string->string);
            }        
        }
        else if (tree->thiscommand.expression.left->type == nul)
        {
            printf("nul\n");
        }

        if (tree->thiscommand.expression.operator != SingleOp)
        {
            printf("Right Operand = ");
            if (tree->thiscommand.expression.right->type == exp)
            {
                printf("tree\n");
                gotoRightSubtree = true; 
            }
            else if (tree->thiscommand.expression.right->type == op)
            {
                printf("Variable %s\n", tree->thiscommand.expression.right->data.op->name->string);
            }
            else if (tree->thiscommand.expression.right->type == imm)
            {
                if (tree->thiscommand.expression.right->data.imm.type == type_int)
                {
                    printf("Imm int %ld \n", tree->thiscommand.expression.right->data.imm.data.type_int);
                }
                else if (tree->thiscommand.expression.right->data.imm.type == type_float)
                {
                    printf("Imm float %f \n", tree->thiscommand.expression.right->data.imm.data.type_float);
                }
                else if (tree->thiscommand.expression.right->data.imm.type == type_string)
                {
                    printf("Imm string %s \n", tree->thiscommand.expression.right->data.imm.data.type_string->string);
                }
            }
            else if (tree->thiscommand.expression.right->type == nul)
            {
                printf("nul\n");
            }
        }
        
        if (gotoLeftSubtree == true)
        {
            printf("Left Subtree\n");
            printTree(tree->thiscommand.expression.left->data.exp);
        }

        if (gotoRightSubtree == true)
        {
            printf("Right Subtree\n");
            printTree(tree->thiscommand.expression.right->data.exp);
        }

        return;

    default:
        printf("\nNeznama noda\n\n");
        break;
    }

    if (tree->nextcommand != NULL)
    {
        printf("\n----------Next Command----------\n");
        printTree(tree->nextcommand);
    }
}




exp_subtree_t* createExpSubtree(symbol_t* symbol, ast_t* subtree, long int* imm_int, double* imm_float, Dyn_String* imm_string)
{
    exp_subtree_t *tree = malloc(sizeof(exp_subtree_t));
    if (tree == NULL) 
    { 
        fprintf(stderr, "malloc error\n"); 
        return NULL; 
    }
    
    if (symbol != NULL && subtree == NULL && imm_int == NULL && imm_float == NULL && imm_string == NULL)
    {
        tree->type = op;
        tree->data.op = symbol;
    }
    else if (symbol == NULL && subtree != NULL && imm_int == NULL && imm_float == NULL && imm_string == NULL)
    {
        tree->type = exp;
        tree->data.exp = subtree;
    }
    else if (symbol == NULL && subtree == NULL && imm_int != NULL && imm_float == NULL && imm_string == NULL)
    {
        tree->type = imm;
        tree->data.imm.type = type_int;
        tree->data.imm.data.type_int = *imm_int;
    }
    else if (symbol == NULL && subtree == NULL && imm_int == NULL && imm_float != NULL && imm_string == NULL)
    {
        tree->type = imm;
        tree->data.imm.type = type_float;
        tree->data.imm.data.type_float = *imm_float;
    }
    else if (symbol == NULL && subtree == NULL && imm_int == NULL && imm_float == NULL && imm_string != NULL)
    {
        tree->type = imm;
        tree->data.imm.type = type_string;
        tree->data.imm.data.type_string = imm_string;
    }
    else if (symbol == NULL && subtree == NULL && imm_int == NULL && imm_float == NULL && imm_string == NULL)
    {
        tree->type = nul;
        tree->data.exp = NULL;
    }
    else
    {
        fprintf(stderr, "spatna kombinace vstupnich parametrů\n");
        return NULL;
    }
    return tree;
}


void diposeExpSubtree(exp_subtree_t* tree)
{
    if (tree == NULL)
    {
        return;
    }

    switch (tree->type)
    {
    case op: case imm: case nul: //simply frees the structure, 
        free(tree);              //in case of op - the symtable record is not touched
        break;
    case exp:
        if (tree->data.exp->thiscommand.expression.operator != SingleOp && tree->data.exp->thiscommand.expression.operator != Error)
        {
            diposeExpSubtree(tree->data.exp->thiscommand.expression.right);
        }
        diposeExpSubtree(tree->data.exp->thiscommand.expression.left);
        free(tree);

    default:
        //shouldn't happen -- error
        break;
    }
}


func_par_t* parInit()
{
    func_par_t *parameters = malloc(sizeof(func_par_t));
    if (parameters == NULL) 
    { 
        fprintf(stderr, "malloc error\n"); 
        return NULL; 
    }

    parameters->type = first;
    parameters->next = NULL;
    return parameters;   
}

func_par_t* addParametrer(func_par_t* parameters, symbol_t* symbol, long int* int_input, double* float_input, Dyn_String* string_input)
{
    if (parameters == NULL)
    {
        return NULL;
    }

    if (parameters->type != first)
    {
        while (parameters->next != NULL)
        {
            parameters = parameters->next;
        }
        parameters->next = malloc(sizeof(func_par_t));
        if (parameters->next == NULL) 
        { 
            fprintf(stderr, "malloc error\n"); 
            return NULL; 
        }
        else 
        { 
            parameters = parameters->next; 
        }
    }
    parameters->next = NULL;
    if (symbol == NULL && int_input == NULL && float_input == NULL && string_input == NULL)
    {
        parameters->type = par_null;
    }
    else if (symbol != NULL && int_input == NULL && float_input == NULL && string_input == NULL)
    {
        parameters->type = par_op;
        parameters->op = symbol;
    }
    else if (symbol == NULL && (int_input != NULL || float_input != NULL || string_input != NULL))
    {
        parameters->type = par_imm;
        if (int_input != NULL && float_input == NULL && string_input == NULL)
        {
            parameters->imm.type = type_int;
            parameters->imm.data.type_int = *int_input;
        }
        else if (int_input == NULL && float_input != NULL && string_input == NULL)
        {
            parameters->imm.type = type_float;
            parameters->imm.data.type_float = *float_input;
        }
        else if (int_input == NULL && float_input == NULL && string_input != NULL)
        {
            parameters->imm.type = type_string;
            parameters->imm.data.type_string = string_input;
        }
    }
    return parameters;
}


void disposeParameters (func_par_t* parameters)
{
    func_par_t* this;
    while (parameters != NULL)
    {
        this = parameters;
        parameters = parameters->next;
        free(this);
    }
}

