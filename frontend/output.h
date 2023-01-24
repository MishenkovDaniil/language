#ifndef OUTPUT_H
#define OUTPUT_H

#include "tree.h"
#include "stack.h"

struct Var
{
    int index = 0;
    char data[20] = "";
};

Var *create_var (const char *var, const int index);

void add_standart (Stack *global, FILE *output);

bool add_struct   (const char *var, Stack *block_names = nullptr);
int  find_var     (const char *var);
int  find_stk_var (const char *var, Stack *block_names);
bool is_my_var    (const char *var, int index, Stack *block_names);

void print_tree    (Tree *tree, FILE *output);

void print_node    (Node *node, FILE *output);

void print_def     (Node *node, FILE *output);
void print_nvar    (Node *node, FILE *output, Stack *block_names = nullptr);
void print_func    (Node *node, FILE *output);

void print_par     (Node *node, FILE *output, Stack *block_names);
void print_arg     (Node *node, FILE *output);

void print_block   (Node *node, FILE *output, Stack *block_names);
void print_seq     (Node *node, FILE *output, Stack *block_names);

void print_ass     (Node *node, FILE *output);
void print_call    (Node *node, FILE *output);
void print_ret     (Node *node, FILE *output);

void print_expr    (Node *node, FILE *output);
void print_op      (Node *node, FILE *output);
void print_var_val (Node *node, FILE *output);
void print_num     (Node *node, FILE *output);

void print_var (Node *node, FILE *output);

#endif /* OUTPUT_H */
