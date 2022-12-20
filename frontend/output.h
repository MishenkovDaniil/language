#ifndef OUTPUT_H
#define OUTPUT_H

#include "tree.h"

void print_tree (Tree *tree, FILE *output);
void print_node (Node *node, FILE *output);
void print_def (Node *node, FILE *output);
void print_nvar (Node *node, FILE *output);
void print_func (Node *node, FILE *output);
void print_expr (Node *node, FILE *output);
void print_num (Node *node, FILE *output);
void print_op (Node *node, FILE *output);
void print_var_val (Node *node, FILE *output);
void print_call (Node *node, FILE *output);
void print_arg (Node *node, FILE *output);
void print_block (Node *node, FILE *output);
void print_seq (Node *node, FILE *output);
void print_ass (Node *node, FILE *output);
void print_ret (Node *node, FILE *output);
void print_par (Node *node, FILE *output);

#endif /* OUTPUT_H */
