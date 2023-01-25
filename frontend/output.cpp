#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "output.h"
#include "stack.h"
#include "input.h"

/// var
/// index
/// shift otn RBP
/// hash (optional)

static int char_index = 1;
//static int var_index = 0;
static int start_capacity = 10;

static Stack names = {};

const int POISON = 0xDEADBEFF;

Var *create_var (const char *var, const int index)
{
    Var *struct_var = (Var *)calloc (1, sizeof (Var));

    strcpy (struct_var->data, var);
    struct_var->index = index;

    return struct_var;
}

void add_standart (Stack *global, FILE *output)
{
    assert (global);

    int max_arg_num = 0;

#define var(name, arg_num, code)                            \
    Var *var_##name = create_var (#name, -1);               \
    stack_push (global, var_##name);                        \
                                                            \
    fprintf (output, #name":\n");                           \
    fprintf (output, code);
#include "standart.h"
#undef var

    //var_index = max_arg_num;
}

bool add_struct (const char *var, bool is_label, Stack *block_names)
{
    assert (var);

    if (!(block_names))
    {
        block_names = (Stack *)stack_get_token (&names, 0);
    }

    int index = find_var (var);

    if (!(index))
    {
        Var *struct_var = nullptr;

        if (is_label)
        {
            struct_var = create_var (var, -1);
        }
        else
        {
            struct_var = create_var (var, char_index++);
        }

        stack_push (block_names, struct_var);

        return true;
    }

    return false;
}

int find_var (const char *var)
{
    size_t num_of_defs = 0;
    int var_index = 0;

    for (int stack_num = 0; stack_num < names.size; stack_num++)
    {
        int founded_idx = find_stk_var (var, (Stack *)(stack_get_token (&names, stack_num)));

        if (founded_idx)
        {
            num_of_defs++;
            var_index = founded_idx;
        }
    }

    if (num_of_defs > 1)
    {
        printf ("Error: multiple definitions of \"%s\"", var);
        return POISON;
    }

    return var_index;
}

int find_stk_var (const char *var, Stack *block_names)
{
    assert (block_names);

    for (int i = 0; i < block_names->size; i++)
    {
        if (is_my_var (var, i, block_names))
        {
            return ((Var *)stack_get_token (block_names, i))->index;
        }
    }

    return 0;
}

bool is_my_var (const char *var, int index, Stack *block_names)
{
    Var *cur_var = (Var *)stack_get_token (block_names, index);

    if (cur_var)
    {
        return strcmp (cur_var->data, var) == 0;
    }

    return 0;
}

void print_tree (Tree *tree, FILE *output)
{
    assert (tree);
    assert (output);

    stack_init (&names, 5);

    Stack global_names = {};
    stack_init (&global_names, 10);

    stack_push (&names, &global_names);


    print_node (tree->root, output);

    stack_dtor (&global_names);
    stack_dtor (&names);
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
    static bool is_first_func = true;

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
            if (is_first_func)
            {
                fprintf (output, "call main\nout\nhlt\n\n");
                add_standart ((Stack *)stack_get_token (&names, 0), output);
                is_first_func = false;
            }

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

void print_nvar (Node *node, FILE *output, Stack *block_names)
{
    add_struct (node->value.var, false, block_names);
    //var_index++;
    print_expr (node->right, output);

    fprintf (output, "pop [%d] /*%s*/\n\n", char_index - 1, node->value.var);
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
    static unsigned int compare_idx = 0;

    const int MAX_COMP_LEN = 8;
    const int MAX_COMP_END_LEN = 12;

    print_expr (node->left, output);
    print_expr (node->right, output);

    switch (node->value.op_val)
    {
        case ADD:
        {
            fprintf (output, "add\n\n");
            break;
        }
        case SUB:
        {
            fprintf (output, "sub\n\n");
            break;
        }
        case MUL:
        {
            fprintf (output, "mult\n\n");
            break;
        }
        case DIV:
        {
            fprintf (output, "div\n\n");
            break;
        }
#define comp_case(comp_op, cmd)                                                                         \
        case comp_op:                                                                                   \
        {                                                                                               \
            char comp[MAX_COMP_LEN] = "";                                                               \
            char comp_end[MAX_COMP_END_LEN] = "";                                                       \
                                                                                                        \
            sprintf (comp, "comp_%d", compare_idx);                                                     \
            sprintf (comp_end, "comp_end_%d", compare_idx);                                             \
                                                                                                        \
            while (!(find_var (comp) == 0 && find_var (comp_end) == 0))                                 \
            {                                                                                           \
                compare_idx++;                                                                          \
                comp[0] = '\0';                                                                         \
                comp_end[0] = '\0';                                                                     \
                                                                                                        \
                sprintf (comp, "comp_%d", compare_idx);                                                 \
                sprintf (comp_end, "comp_end_%d", compare_idx);                                         \
            }                                                                                           \
                                                                                                        \
            add_struct (comp, true);                                                                    \
            add_struct (comp_end, true);                                                                \
                                                                                                        \
            fprintf (output, #cmd" comp_%d\n"                                                           \
                             "push 0\n"                                                                 \
                             "jmp comp_end_%d\n\n"                                                      \
                             "comp_%d:\n"                                                               \
                             "push 1\n"                                                                 \
                             "comp_end_%d:\n\n", compare_idx, compare_idx, compare_idx, compare_idx);   \
            compare_idx++;                                                                              \
            break;                                                                                      \
        }

        comp_case (LT, jb)
        comp_case (GT, ja)
        comp_case (LEQ, jbe)
        comp_case (GEQ, jae)
        comp_case (EQ, je)
        comp_case (NEQ, jne)
#undef comp_case
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
        fprintf (stderr, "Error: variable {%s} was not defined.(%s)\n", node->value.var, __PRETTY_FUNCTION__);
        return;
    }
    else if (i == -1)
    {
        fprintf (stderr, "Error: variable {%s} is multiple defined.\n", node->value.var);
        return;
    }

    fprintf (output, "push [%d] /*%s*/\n", i, node->value.var);
    //fprintf (output, "dup\n");
    //fprintf (output, "pop [%d] /*%s*/\n", i, node->value.var);
}

void print_call (Node *node, FILE *output)
{
    print_arg (node->right, output);

    //if (find_var(node->value.var) != 1)
    //{
      //  fprintf (stderr, "Error: func '%s' was not inited.\n", node->value.var);
    //}

    fprintf (output, "call %s\n\n", node->value.var);
}

void print_arg (Node *node, FILE *output)
{
    if (!(node))
    {
        return;
    }

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
    if (!(add_struct (node->value.var, true)))
    {
        //error
        fprintf (stderr, "Syntax error: function '%s' declared more than once.\n", node->value.var);
        return;
    }

    fprintf (output, "\n%s:\n\n", node->value.var);

    Stack func = {};
    stack_init (&func, start_capacity);

    stack_push (&names, &func);

    if (node->left)
    {
        print_par (node->left, output, &func);
    }

    if (!(node->right))
    {
        fprintf (stderr, "Syntax error: func has no sense");
        return;
    }

    print_block (node->right, output, &func);

    stack_pop (&names);
    stack_dtor (&func);
}

void print_par (Node *node, FILE *output, Stack *block_names)
{
    if (!(node))
    {
        return;
    }

    if (!(add_struct (node->value.var, false, block_names)))
    {
        return;
    }

    int i = char_index - 1;

    print_par (node->right, output, block_names);
//    int i = find_var (node->value.var);//?? works??

    fprintf (output, "pop [%d] /*%s*/\n", i, node->value.var);//i?
}

void print_block (Node *node, FILE *output, Stack *block_names)
{
    if (node->left)
    {
        print_seq (node->left, output, block_names);
    }
    if (node->right)
    {
        print_block (node->right, output, block_names);
    }
}

void print_var (Node *node, FILE *output)
{
    int i = find_var (node->value.var);

    if (i == 0)
    {
        fprintf (stderr, "Error: variable {%s} was not defined.(%s)\n", node->value.var, __PRETTY_FUNCTION__);
        return;
    }
    else if (i == -1)
    {
        fprintf (stderr, "Error: variable {%s} is multiple defined.\n", node->value.var);
        return;
    }

    fprintf (output, "push [%d] /*%s*/\n", i, node->value.var);
}

void print_seq (Node *node, FILE *output, Stack *block_names)
{
    if (!(node))
    {
        return;
    }

    switch (node->type)
    {
        case NVAR:
        {
            print_nvar (node, output, block_names);
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
            print_if (node, output);
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
            debug_print ("Error: unknown action");
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

    if (i == 0)
    {
        fprintf (stderr, "Error: variable {%s} was not defined.(%s)\n", node->left->value.var, __PRETTY_FUNCTION__);
        return;
    }
    else if (i == -1)
    {
        fprintf (stderr, "Error: variable {%s} is multiple defined.\n", node->left->value.var);
        return;
    }

    fprintf (output, "pop [%d] /*%s*/\n", i, node->left->value.var);
}

void print_ret (Node *node, FILE *output)
{
    print_expr (node->right, output);

    fprintf (output, "RET\n\n");
}

void print_if (Node *node, FILE *output)
{
    printf ("if\n");

    static unsigned int if_idx = 0;

    fprintf (output, "/*if_%d*/\n\n", if_idx);

    print_expr (node->left, output);

    fprintf (output, "push 0\n"
                     "je if_end_%d\n\n", if_idx, if_idx);

    print_branch (node->right->left, output);

    fprintf (output, "if_end_%d:\n\n", if_idx++);

    printf ("if end\n");
}

void print_branch (Node *node, FILE *output)
{
    if (node && node->type == SEQ)
    {
        print_seq (node->left, output, (Stack *)stack_get_token (&names, 0));
        print_branch (node->right, output);
    }
}

