#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "output.h"
#include "stack.h"

/// var
/// index
/// shift otn RBP
/// hash (optional)

static int char_index = 1;

Stack chars = {};

void add_struct (const char *var)
{
    if (!(find_var (var)))
    {
        Var *struct_var = (Var *)calloc (1, sizeof (Var));

        struct_var->data = var;

        stack_push (&chars, struct_var);

        char_index++;
    }
}

int find_var (const char *var)
{
    for (int i = 1; i < char_index; i++)
    {
        if (is_my_var (var, i))
        {
            return i;
        }
    }

    return 0;
}

bool is_my_var (const char *var, int index)
{
    Var *cur_var = (Var *)stack_get_token (&chars, index);

    if (cur_var)
    {
        return strcmp (cur_var->data, var) == 0;
    }

    return 0;
}

void print_tree (Tree *tree, FILE *output)
{
    fprintf (output, "jmp main\n");

    stack_init (&chars, 10);

    print_node (tree->root, output);
    stack_dtor (&chars);
}

void print_node (Node *node, FILE *output)
{
    if (!(node))
    {
        return;
    }

    print_def (node->left, output);
    print_node (node->right, output);
}

void print_def (Node *node, FILE *output)
{
    if (!(node))
    {
        return;
    }

    switch (node->type)
    {
        case NVAR:
        {
            print_nvar (node, output);
            break;
        }
        case NFUN:
        {
            print_func (node, output);
            break;
        }
        default:
        {
            fprintf (output, "Error: not def type in %s.\n", __PRETTY_FUNCTION__);
            break;
        }
    }
}
void print_nvar (Node *node, FILE *output)
{
    print_expr (node->right, output);

    add_struct (node->value.var);

    fprintf (output, "pop [%d]\\\\ %s\n", char_index - 1, node->value.var);
}

void print_expr (Node *node, FILE *output)
{
    if (!(node))
    {
        return;
    }

    switch (node->type)
    {
        case CONST:
        {
            print_num (node, output);
            break;
        }
        case OP:
        {
            print_op (node, output);
            break;
        }
        case VAR:
        {
            print_var_val (node, output);
            break;
        }
        case CALL:
        {
            print_call (node, output);
            break;
        }
        default:
        {
            fprintf (stderr, "Error: not expr type.\n");
            break;
        }
    }
}

void print_num (Node *node, FILE *output)
{
    fprintf (output, "push %d\n", (int)node->value.dbl_val);
}

void print_op (Node *node, FILE *output)
{
    print_expr (node->left, output);
    print_expr (node->right, output);

    switch (node->value.op_val)
    {
        case ADD:
        {
            fprintf (output, "add\n");
            break;
        }
        case SUB:
        {
            fprintf (output, "sub\n");
            break;
        }
        case MUL:
        {
            fprintf (output, "mult\n");
            break;
        }
        case DIV:
        {
            fprintf (output, "div\n");
            break;
        }
        default:
        {
            fprintf (output, "Error: unknown operation");
            break;
        }
    }
}
void print_var_val (Node *node, FILE *output)
{
    int i = find_var (node->value.var);

    if (i == 0)
    {
        fprintf (stderr, "Error: variable {%s} was not defined", node->value.var);
        return;
    }

    fprintf (output, "push [%d]\\\\%s\n", i, node->value.var);
    fprintf (output, "dup\n");
    fprintf (output, "pop [%d]\\\\%s\n", i, node->value.var);
}

void print_call (Node *node, FILE *output)
{
    print_arg (node->right, output);
    fprintf (output, "call %s\n", node->value.var);
}

void print_arg (Node *node, FILE *output)
{
    if (node->left)
    {
        print_expr (node->left, output);
    }
    if (node->right)
    {
        print_arg (node->right, output);
    }
}

void print_func (Node *node, FILE *output)
{
    fprintf (output, "\n%s:\n", node->value.var);

    if (node->left)
    {
        print_par (node->left, output);
    }

    if (!(node->right))
    {
        fprintf (stderr, "Syntax error: func has no sense");
        return;
    }

    print_block (node->right, output);
}

void print_par (Node *node, FILE *output)
{
    if (!(node))
    {
        return;
    }

    add_struct (node->value.var);//

    print_par (node->right, output);
    int i = find_var (node->value.var);//?? works??

    fprintf (output, "pop [%d]\\\\%s\n", i, node->value.var);//i?
}

void print_block (Node *node, FILE *output)
{
    if (node->left)
    {
        print_seq (node->left, output);
    }
    if (node->right)
    {
        print_block (node->right, output);
    }
}

void print_var (Node *node, FILE *output)
{
    int i = find_var (node->value.var);

    fprintf (output, "push [%d]\\\\%s\n", i, node->value.var);
}


void print_seq (Node *node, FILE *output)
{
    if (!(node))
    {
        return;
    }

    switch (node->type)
    {
        case NVAR:
        {
            print_nvar (node, output);
            break;
        }
        case CONST:
        {
            print_num (node, output);
            break;
        }
        case ASS:
        {
            print_ass (node, output);
            break;
        }
        case VAR:
        {
            print_var (node, output);
            break;
        }
        case IF:
        {
            //print_if (node, output);
            break;
        }
        case WHILE:
        {
            //print_while (node, output);
            break;
        }
        case RET:
        {
            print_ret (node, output);
            break;
        }
        case CALL:
        {
            print_call (node, output);
            break;
        }
        default:
        {
            fprintf (stderr, "Error: unknown action");
            break;
        }
    }
}

void print_ass (Node *node, FILE *output)
{
    print_expr (node->right, output);

    if (!(node->left))
    {
        fprintf (stderr, "Error\n");
        return;
    }

    int i = find_var (node->left->value.var);
    fprintf (output, "pop [%d]\\\\%s\n", i, node->left->value.var);
}

void print_ret (Node *node, FILE *output)
{
    print_expr (node->right, output);

    fprintf (output, "RET\n");
}
