/*
    Grammar  ::= Defs
    Defs     ::= (Nvar | Nfun) Defs*

    Nvar     ::= Name Expr
    Nfun     ::= Name Par* Block
    Par      ::= Name

    Block    ::= Seq
    Seq      ::= (Block | Nvar | Ass | If | While | Call | Ret) Seq*
    Ass      ::= Var Expr
    If       ::= "if"     '(' Expr ')'
    While    ::= "while"  '(' Expr ')'
    Call     ::= Name Arg
    Arg      ::= Const Arg*
    Ret      ::= Expr

    Op       ::= '+' | '-' | '*' | '/' | '^'
    Expr     ::= T (['+' | '-'] T)*
    T        ::= Deg (['*' | '/'] Deg)*
    Deg      ::= Primary ('^' Primary)*
    Primary  ::= '('Expr')' | Var | Const | Call
    Name     ::= ['a'-'z','0'-'9','_']+                    ///not initialized before
    Var      ::= ['a'-'z', '_'] ['a'-'z','0'-'9','_']+     ///already initialized
    Const    ::= ['0'-'9']+
*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexems.h"
#include "tree.h"
#include "input.h"
#include "op_types.h"

#define stack_lexem(index)              ((Lexem *)stack_get_token (lexems, index))
#define create_num(num)                 tree_create_node (TYPE_NUM, #num)
#define Left                            node->left
#define Right                           node->right
#define cL                              cpy_node (Left)
#define cR                              cpy_node (Right)
#define ADD(left_node, right_node)      tree_create_node (OP, "ADD", left_node, right_node)
#define SUB(left_node, right_node)      tree_create_node (OP, "SUB", left_node, right_node)
#define MUL(left_node, right_node)      tree_create_node (OP, "MUL", left_node, right_node)
#define DIV(left_node, right_node)      tree_create_node (OP, "DIV", left_node, right_node)
#define DEG(left_node, right_node)      tree_create_node (OP, "DEG", left_node, right_node)
#define GT(left_node, right_node)       tree_create_node (OP, "GT", left_node, right_node)
#define LT(left_node, right_node)       tree_create_node (OP, "LT", left_node, right_node)

//#define SIN(node)                       tree_create_node (OP, "SIN", nullptr, node)
//#define COS(node)                       tree_create_node (OP, "COS", nullptr, node)

Node *tree_fill (Tree *tree, Stack *lexems)
{
    GetNodeG (tree, lexems);

    if (!(tree->root))
    {
        debug_print ("Error: failed to read from input file.\n");
    }

    return tree->root;
}

Node *GetNodeG (Tree *tree, Stack *lexems)
{
    int index = 0;
    tree->root = GetNodeDefs (lexems, &index);
    swap_defs (tree->root);

    if (!(stack_lexem(index) == nullptr))
    {
        debug_print ("Error: last symbol to read is not '\\0'.\n");
        return nullptr;
    }/*
    if (lexems[index]->type != L_SEQ)
    {
        debug_print ("Error: last symbol to read is not '\\0' (expr is  now)");
        return nullptr;
    }
*/
    return tree->root;
}

Node *GetNodeDefs (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = DEFS;

    Lexem *cur_lexem = stack_lexem(*index);

    if (cur_lexem == nullptr)
    {
        return nullptr;
    }
    if (cur_lexem->type == L_DEFAULT)
    {
        return nullptr;
    }

    if (cur_lexem->type == L_SEQ)
    {
        (*index)++;
        result = GetNodeDefs (lexems, index);
    }
    else if (cur_lexem->type == L_NFUN || cur_lexem->type == L_NVAR)
    {
        result = (Node *)calloc (1, sizeof (Node));
        result->type = DEFS;

        if (cur_lexem->type == L_NVAR)
        {
            result->left = GetNodeNvar (lexems, index);
        }
        else
        {
            result->left = GetNodeNfun (lexems, index);
        }

        result->right = GetNodeDefs (lexems, index);
    }

    return result;
}

void swap_defs (Node *node)
{
    for (int i = 0; i < 2; i++)
    {
        for (Node *def = node; def != nullptr; def = def->right)
        {
            if (def->right != nullptr && def->right->left != nullptr && def->right->left->type == NVAR && def->left->type == NFUN)
            {
                Node *temp = def->right->left;
                def->right->left = def->left;
                def->left = temp;
            }
        }
    }
}

Node *GetNodeNfun (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));

    result->type = NFUN;
    result->value.var = stack_lexem(*index)->value.var;

    (*index)++;

    if (stack_lexem(*index)->type == L_STARTING_BRACKET)
    {
        (*index)++;

        result->left = GetNodePar (lexems, index);
        if (stack_lexem(*index)->type != L_CLOSING_BRACKET)
        {
            debug_print ("Syntax error: no closing bracket in args of func %s.\n", result->value.var);
            return nullptr;
        }

        (*index)++;
    }

    if (stack_lexem(*index)->type != L_BLOCK_START)
    {
        debug_print ("Syntax error: no '{' after function defining.\n");
        return nullptr;
    }

    (*index)++;

    result->right = GetNodeBlock (lexems, index);

    return result;
}

Node *GetNodePar (Stack *lexems, int *index)
{
    Node *result = nullptr;

    if (stack_lexem(*index)->type == L_VAR)
    {
        result = (Node *)calloc (1, sizeof (Node));
        result->type = PAR;
        result->value.var = stack_lexem(*index)->value.var;

        (*index)++;

        result->right = GetNodePar (lexems, index);
    }

    return result;
}

Node *GetNodeBlock (Stack *lexems, int *index)
{
    Node *result = GetNodeSeq (lexems, index);

    if (stack_lexem(*index)->type != L_BLOCK_END)
    {
        debug_print ("no closing '}', lexem type is {%d}.\n", stack_lexem(*index)->type);

        return nullptr;
    }

    (*index)++;

    return result;
}

Node *GetNodeBranch (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = BRANCH;

    switch (stack_lexem(*index)->type)
    {
        case L_BLOCK_START:
        {
            (*index)++;
            result->left = GetNodeBlock (lexems, index);
            break;
        }
        case L_VAR:
        {
            result->left = GetNodeAss (lexems, index);
            break;
        }
        case L_IF:
        {
            result->left = GetNodeIf (lexems, index);
            break;
        }
        case L_WHILE:
        {
            result->left = GetNodeWhile (lexems, index);
            break;
        }
        case L_RET:
        {
            result->left = GetNodeRet (lexems, index);
            break;
        }
        case L_CALL:
        {
            result->left = GetNodeCall (lexems, index);
            break;
        }
        default:
        {
            result->left = nullptr;
            break;
        }
    }

    if (result->left != nullptr && (result->left->type == IF || result->left->type == WHILE)  && stack_lexem(*index) != nullptr)
    {
        result->right = GetNodeBranch (lexems, index);
    }

    return result;
}

Node *GetNodeSeq (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = SEQ;

    switch (stack_lexem(*index)->type)
    {
        case L_BLOCK_START:
        {
            (*index)++;
            result->left = GetNodeBlock (lexems, index);

            break;
        }
        case L_NVAR:
        {
            result->left = GetNodeNvar (lexems, index);

            break;
        }
        case L_VAR:
        {
            result->left = GetNodeAss (lexems, index);
            break;
        }
        case L_IF:
        {
            result->left = GetNodeIf (lexems, index);
            break;
        }
        case L_WHILE:
        {
            result->left = GetNodeWhile (lexems, index);
            break;
        }
        case L_RET:
        {
            result->left = GetNodeRet (lexems, index);
            break;
        }
        case L_CALL:
        {
            result->left = GetNodeCall (lexems, index);
            break;
        }
        default:
        {
            result = nullptr;
            break;
        }
    }

    if (result && stack_lexem(*index) != nullptr && (stack_lexem(*index)->type == L_SEQ || stack_lexem((*index) - 1)->type == L_BLOCK_END))
    {
        int index_t = stack_lexem(*index)->type;
        int pre_index_t = stack_lexem((*index) - 1)->type;

        if (index_t == L_SEQ || pre_index_t == L_BLOCK_END)
        {
            if ((index_t == L_SEQ && pre_index_t == L_BLOCK_END) || result->left == nullptr)
            {
                debug_print ("Syntax error: no command before ';'.\n");
                return nullptr;
            }
            else if (index_t == L_SEQ)
            {
                (*index)++;
            }

            result->right = GetNodeSeq (lexems, index);
        }
    }

    return result;
}

Node *GetNodeNvar (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = NVAR;
    result->value.var = stack_lexem(*index)->value.var;

    (*index)++;

    result->right = GetNodeE(lexems, index);

    return result;
}

Node *GetNodeAss (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));

    result->type = ASS;

    result->left = GetNodeV (lexems, index);
    (*index)++;

    result->right = GetNodeE (lexems, index);

    return result;
}

Node *GetNodeRet (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = RET;

    (*index)++;

    result->right = GetNodeE (lexems, index);
    return result;
}

Node *GetNodeCall (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = CALL;

    result->value.var = stack_lexem(*index)->value.var;
    (*index)++;

    if (stack_lexem(*index)->type == L_STARTING_BRACKET)
    {
        (*index)++;

        result->right = GetNodeArg (lexems, index);

        if (stack_lexem(*index)->type != L_CLOSING_BRACKET)
        {
            debug_print ("Syntax error: no closing bracket in args of func %s.\n", result->value.var);
            return nullptr;
        }

        (*index)++;
    }

    return result;
}

Node *GetNodeArg (Stack *lexems, int *index)
{
    Node *result = nullptr;

    Lexem *cur_lexem = stack_lexem(*index);

    if (cur_lexem->type == L_NUM || cur_lexem->type == L_VAR || cur_lexem->type == L_OP || cur_lexem->type == L_CALL)
    {
        result = (Node *)calloc (1, sizeof (Node));
        assert (result);

        result->type = ARG;

        result->left = GetNodeE (lexems, index);
        result->right = GetNodeArg (lexems, index);
    }

    return result;
}

Node *GetNodeIf (Stack *lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = IF;

    //result->value.var = stack_lexem(*index)->value.var;
    (*index)++;

    if (stack_lexem(*index)->type == L_STARTING_BRACKET)
    {
        (*index)++;
        result->left = GetNodeE (lexems, index);

        if (stack_lexem(*index)->type != L_CLOSING_BRACKET)
        {
            debug_print ("Syntax error: no closing bracket in 'if' condition %s.\n", result->value.var);
            return nullptr;
        }

        (*index)++;

        result->right = GetNodeBranch (lexems, index);
    }
    else
    {
        printf ("Error: no if condition.\n");
        return nullptr;
    }
    return result;
}

Node *GetNodeWhile (Stack *lexems, int *index)
{
    return nullptr;
}

Node *GetNodeE (Stack *lexems, int *index)
{
    Node *result = GetNodeF (lexems, index);
    Node *right_node = nullptr;

    Lexem *cur_lexem = stack_lexem(*index);

    while (cur_lexem->type == L_OP &&
          (cur_lexem->value.op_val == GT ||
           cur_lexem->value.op_val == LT))
    {
        Op_types op = cur_lexem->value.op_val;

        (*index)++;

        right_node = GetNodeT (lexems, index);

        if (op == GT)
        {
            result = GT (result, right_node);
        }
        else
        {
            result = LT (result, right_node);
        }

        cur_lexem = stack_lexem(*index);
    }

    return result;
}

Node *GetNodeF (Stack *lexems, int *index)
{
    Node *result = GetNodeT (lexems, index);
    Node *right_node = nullptr;

    Lexem *cur_lexem = stack_lexem(*index);

    while (cur_lexem->type == L_OP &&
          (cur_lexem->value.op_val == ADD ||
           cur_lexem->value.op_val == SUB))
    {
        Op_types op = cur_lexem->value.op_val;

        (*index)++;

        right_node = GetNodeT (lexems, index);

        if (op == ADD)
        {
            result = ADD (result, right_node);
        }
        else
        {
            result = SUB (result, right_node);
        }

        cur_lexem = stack_lexem(*index);
    }

    return result;
}

Node *GetNodeT (Stack *lexems, int *index)
{
    Node *result = GetNodeD (lexems, index);
    Node *right_node = nullptr;

    Lexem *cur_lexem = stack_lexem(*index);

    while (cur_lexem->type == L_OP &&
          (cur_lexem->value.op_val == MUL ||
           cur_lexem->value.op_val == DIV))
    {
        Op_types op = cur_lexem->value.op_val;

        (*index)++;

        right_node = GetNodeD (lexems, index);

        if (op == MUL)
        {
            result = MUL (result, right_node);
        }
        else
        {
            result = DIV (result, right_node);
        }

        cur_lexem = stack_lexem(*index);
    }

    return result;
}

Node *GetNodeD (Stack *lexems, int *index)
{
    Node *result = GetNodeP (lexems, index);
    Node *right_node = nullptr;

    Lexem *cur_lexem = stack_lexem(*index);

    while (cur_lexem->type == L_OP &&
           cur_lexem->value.op_val == DEG)
    {
        (*index)++;

        right_node = GetNodeP (lexems, index);

        result = DEG (result, right_node);

        cur_lexem = stack_lexem(*index);
    }

    return result;
}

Node *GetNodeP (Stack *lexems, int *index)
{
    Node *result = nullptr;

    Lexem *cur_lexem = stack_lexem(*index);

    if (cur_lexem->type == L_STARTING_BRACKET)
    {
        (*index)++;

        result = GetNodeE (lexems, index);

        if (!(stack_lexem(*index)->type == L_CLOSING_BRACKET))
        {
            debug_print ("Error: no closing bracket (after opening one).\n");
            return nullptr;
        }

        (*index)++;
    }
    else if (cur_lexem->type == L_CALL)
    {
        result = GetNodeCall (lexems, index);
    }
    else if (!(result = GetNodeV (lexems, index)))
    {
        result = GetNodeN (lexems, index);
    }

    if (!(result))
    {
        debug_print ("Syntax error: no matching lexem.\n");
        return nullptr;
    }

    return result;
}

Node *GetNodeN (Stack *lexems, int *index)
{
    Lexem *cur_lexem = stack_lexem(*index);

    if (cur_lexem->type == L_NUM)
    {
        Node *result = (Node *)calloc (1, sizeof(Node));

        result->type = CONST;
        result->value.dbl_val = cur_lexem->value.dbl_val;

        (*index)++;

        return result;
    }
    else
    {
        debug_print ("Error:not num in GetNum function %d.\n", cur_lexem->type);
    }

    return nullptr;
}

Node *GetNodeV (Stack *lexems, int *index)
{
    Lexem *cur_lexem = stack_lexem(*index);

    if (cur_lexem->type == L_VAR)
    {
        Node *result = (Node *)calloc (1, sizeof(Node));

        result->type = VAR;
        result->value.var = cur_lexem->value.var;

        (*index)++;

        return result;
    }

    return nullptr;
}

#undef create_num
#undef Left
#undef Right
#undef dL
#undef dR
#undef cL
#undef cR
#undef ADD
#undef SUB
#undef MUL
#undef DIV
#undef DEG
#undef SIN
#undef COS
#undef debug_print
#undef tex_print
