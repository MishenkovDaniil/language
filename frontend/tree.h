#ifndef LANGUAGE_TREE_H
#define LANGUAGE_TREE_H

#include "op_types.h"

enum Type
{
    DEFAULT = 0,

    DEFS   = 1,
    NVAR   = 2,
    NFUN   = 3,
    BLOCK  = 4,
    ARG    = 5,
    OP     = 6,
    SEQ    = 7,
    ASS    = 8,
    WHILE  = 9,
    IF     = 10,
    BRANCH = 11,
    CALL   = 12,
    PAR    = 13,
    RET    = 14,
    CONST  = 15,
    VAR    = 16,
};

union Value
{
    double dbl_val;
    char *var;
    Op_types op_val;
};

struct Node
{
    Type type = DEFAULT;

    Value value = {};

    Node *left = nullptr;
    Node *right = nullptr;
    Node *parent = nullptr;
};

struct Tree
{
    Node *root = nullptr;//add debug info
};


static unsigned int DEF_ERRNO = 0;

Node *tree_create_node (Type type, const char *value, Node *left = nullptr, Node *right = nullptr);
Node *tree_create_num (double val, Node *left = nullptr, Node *right = nullptr);

void swap_nodes (Node **first_node, Node **second_node);

void tree_ctor (Tree *tree, unsigned int *err = &DEF_ERRNO);
void add_nodes (Tree *tree, Node *parent_node, Type left_type, Type right_type,
               char *left_value, char *right_value, unsigned int *err = &DEF_ERRNO);
void tree_dtor (Tree *tree, unsigned int *err = &DEF_ERRNO);
void tree_free (Node *node);

unsigned int tree_check (Tree *tree, unsigned int *err);
void tree_dump (Tree *tree, unsigned int *err);
void tree_graph (Tree *tree);
int make_graph_nodes (Node *node, FILE *tgraph_file);

#endif /* LANGUAGE_TREE_H */
