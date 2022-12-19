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
//#define SIN(node)                       tree_create_node (OP, "SIN", nullptr, node)
//#define COS(node)                       tree_create_node (OP, "COS", nullptr, node)

Node *tree_fill (Tree *tree, Lexem **lexems)
{
    GetNodeG (tree, lexems);

    if (!(tree->root))
    {
        debug_print ("Error: failed to read from input file");
    }

    return tree->root;
}

Node *GetNodeG (Tree *tree, Lexem **lexems)
{
    int index = 0;
    tree->root = GetNodeDefs (lexems, &index);

    if (!(lexems[index] == nullptr))
    {
        debug_print ("Error: last symbol to read is not '\\0' (expr is  now)");
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

Node *GetNodeDefs (Lexem **lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = DEFS;

    if (lexems[*index] == nullptr)
    {
        return nullptr;
    }
    if (lexems[*index]->type == L_DEFAULT)
    {
        return nullptr;
    }
    if (lexems[*index]->type == L_SEQ)
    {
        (*index)++;
    }

    if (lexems[*index]->type == L_NFUN || lexems[*index]->type == L_NVAR)
    {
        if (lexems[*index]->type == L_NVAR)
        {
            result->left = GetNodeNvar (lexems, index);
        }
        else
        {
            result->left = GetNodeNfun (lexems, index);
        }

        result->right = GetNodeDefs (lexems, index);
    }
    else
    {
        return nullptr;
    }

    return result;
}

Node *GetNodeNfun (Lexem **lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));

    result->type = NFUN;
    result->value.var = lexems[*index]->value.var;

    (*index)++;

    if (lexems[*index]->type == L_STARTING_BRACKET)
    {
        (*index)++;

        result->left = GetNodePar (lexems, index);
        if (lexems[*index]->type != L_CLOSING_BRACKET)
        {
            debug_print ("Syntax error: no closing bracket in args of func %s.\n", result->value.var);
            return nullptr;
        }

        (*index)++;
    }

    if (lexems[*index]->type != L_BLOCK_START)
    {
        debug_print ("Syntax error: no '{' after function defining.\n");
        return nullptr;
    }

    (*index)++;

    result->right = GetNodeBlock (lexems, index);

    return result;
}

Node *GetNodePar (Lexem **lexems, int *index)
{
    Node *result = nullptr;

    if (lexems[*index]->type == L_VAR)
    {
        result = (Node *)calloc (1, sizeof (Node));
        result->type = PAR;
        result->value.var = lexems[*index]->value.var;

        (*index)++;

        result->right = GetNodePar (lexems, index);
    }

    return result;
}

Node *GetNodeBlock (Lexem **lexems, int *index)
{
    Node *result = GetNodeSeq (lexems, index);

    if (lexems[*index]->type != L_BLOCK_END)
    {
        debug_print ("no closing '}'.\n");

        return nullptr;
    }

    (*index)++;

    return result;
}

Node *GetNodeSeq (Lexem **lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = SEQ;

    switch (lexems[*index]->type)
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
            result->left = nullptr;
            break;
        }
    }

    if (result->left != nullptr && lexems[*index] != nullptr && lexems[*index]->type == L_SEQ)
    {
        (*index)++;
        result->right = GetNodeSeq (lexems, index);
    }

    return result;
}

Node *GetNodeNvar (Lexem **lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = NVAR;
    result->value.var = lexems[*index]->value.var;

    (*index)++;

    result->right = GetNodeE(lexems, index);

    return result;
}

Node *GetNodeAss (Lexem **lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));

    result->type = ASS;

    result->left = GetNodeV (lexems, index);
    (*index)++;

    result->right = GetNodeE (lexems, index);

    return result;
}

Node *GetNodeRet (Lexem **lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = RET;

    (*index)++;

    result->right = GetNodeE (lexems, index);
    return result;
}

Node *GetNodeCall (Lexem **lexems, int *index)
{
    Node *result = (Node *)calloc (1, sizeof (Node));
    result->type = CALL;

    result->value.var = lexems[*index]->value.var;
    (*index)++;

    if (lexems[*index]->type == L_STARTING_BRACKET)
    {
        (*index)++;

        result->left = GetNodeArg (lexems, index);

        if (lexems[*index]->type != L_CLOSING_BRACKET)
        {
            debug_print ("Syntax error: no closing bracket in args of func %s.\n", result->value.var);
            return nullptr;
        }

        (*index)++;
    }

    return result;
}

Node *GetNodeArg (Lexem **lexems, int *index)
{
    Node *result = nullptr;

    result = (Node *)calloc (1, sizeof (Node));
    assert (result);
    result->type = ARG;

    if (lexems[*index]->type == L_NUM || lexems[*index]->type == L_VAR || lexems[*index]->type == L_OP || lexems[*index]->type == L_CALL)
    {
        result->left = GetNodeE (lexems, index);
        result->right = GetNodeArg (lexems, index);
    }

    return result;
}
Node *GetNodeIf (Lexem **lexems, int *index)
{
    return nullptr;
}
Node *GetNodeWhile (Lexem **lexems, int *index)
{
    return nullptr;
}

Node *GetNodeE (Lexem **lexems, int *index)
{
    Node *result = GetNodeT (lexems, index);
    Node *right_node = nullptr;

    while (lexems[*index]->type == L_OP &&
          (lexems[*index]->value.op_val == ADD ||
           lexems[*index]->value.op_val == SUB))
    {
        Op_types op = lexems[*index]->value.op_val;

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
    }

    return result;
}

Node *GetNodeT (Lexem **lexems, int *index)
{
    Node *result = GetNodeD (lexems, index);
    Node *right_node = nullptr;

    while (lexems[*index]->type == L_OP &&
          (lexems[*index]->value.op_val == MUL ||
           lexems[*index]->value.op_val == DIV))
    {
        Op_types op = lexems[*index]->value.op_val;

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
    }

    return result;
}

Node *GetNodeD (Lexem **lexems, int *index)
{
    Node *result = GetNodeP (lexems, index);
    Node *right_node = nullptr;

    while (lexems[*index]->type == L_OP &&
           lexems[*index]->value.op_val == DEG)
    {
        (*index)++;

        right_node = GetNodeP (lexems, index);

        result = DEG (result, right_node);
    }

    return result;
}

Node *GetNodeP (Lexem **lexems, int *index)
{
    Node *result = nullptr;

    if (lexems[*index]->type == L_STARTING_BRACKET)
    {
        (*index)++;

        result = GetNodeE (lexems, index);

        if (!(lexems[*index]->type == L_CLOSING_BRACKET))
        {
            debug_print ("Error: no closing bracket (after opening one) (expr is  now)");
            return nullptr;
        }

        (*index)++;
    }
    else if (lexems[*index]->type == L_CALL)
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

Node *GetNodeN (Lexem **lexems, int *index)
{
    if (lexems[*index]->type == L_NUM)
    {
        Node *result = (Node *)calloc (1, sizeof(Node));

        result->type = CONST;
        result->value.dbl_val = lexems[*index]->value.dbl_val;

        (*index)++;

        return result;
    }
    else
    {
        debug_print ("Error:not num in GetNum function");
    }

    return nullptr;
}

Node *GetNodeV (Lexem **lexems, int *index)
{
    if (lexems[*index]->type == L_VAR)
    {
        Node *result = (Node *)calloc (1, sizeof(Node));

        result->type = VAR;
        result->value.var = lexems[*index]->value.var;

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
