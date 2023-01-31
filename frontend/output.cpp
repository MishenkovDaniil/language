#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "tree.h"
#include "output.h"
#include "stack.h"
#include "input.h"
#include "cctype"
/// var
/// index
/// shift otn RBP
/// hash (optional)

#define global_stk (Stack *)stack_get_token (&names, 0)

static int char_index = 1;
//static int var_index = 0;
static int start_capacity = 10;

static Stack names = {};

static const int L_POISON = 0xDEADBEFF;

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

    //int max_arg_num = 0;

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

    if (var_index == L_POISON || num_of_defs > 1)
    {
        fprintf (stderr, "Error: multiple definitions of \"%s\"", var);
        return L_POISON;
    }

    return var_index;
}

int find_stk_var (const char *var, Stack *block_names)
{
    assert (block_names);

    size_t num_of_defs = 0;
    int var_index = 0;

    for (int i = 0; i < block_names->size; i++)
    {
        if (is_my_var (var, i, block_names))
        {
            int founded_idx = ((Var *)stack_get_token (block_names, i))->index;
            var_index = founded_idx;

            num_of_defs++;
        }
    }

    if (num_of_defs > 1)
    {
        fprintf (stderr, "Error: multiple definitions of \"%s\"", var);
        return L_POISON;
    }

    return var_index;
}

bool is_my_var (const char *var, int index, Stack *block_names)
{
    Var *cur_var = (Var *)stack_get_token (block_names, index);

    if (cur_var)
    {
        return (strcmp (cur_var->data, var) == 0);
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
                fprintf (output, "push 0\n"
                                 "pop RAX\n"
                                 "call main\n"
                                 "out\n"
                                 "hlt\n\n");
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
    if (!(add_struct (node->value.var, false, block_names)))
    {
        fprintf (stderr, "Syntax error: variable '%s' declared more than once.\n", node->value.var);
        return;
    }
    //var_index++;
    print_expr (node->right, output, block_names);

    fprintf (output, "pop ");
    print_var_idx (char_index - 1, block_names, output, node->value.var);
    //fprintf (output, "pop [%d] /*%s*/\n\n", char_index - 1, node->value.var);
}

void print_expr (Node *node, FILE *output, Stack *block_names)
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
            print_op (node, output, block_names);
            break;
        }
        case VAR:
        {
            print_var_val (node, output, block_names);
            break;
        }
        case CALL:
        {
            print_call (node, output, block_names);
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

void print_op (Node *node, FILE *output, Stack *block_names)
{
    static unsigned int compare_idx = 0;

    const int MAX_OP_LEN = 4;
    const int MAX_COMP_LEN = 8;
    const int MAX_COMP_END_LEN = 12;

    print_expr (node->left, output, block_names);
    print_expr (node->right, output, block_names);

    switch (node->value.op_val)
    {
        #define DEF_OP(name,...)            \
        case name:                          \
        {                                   \
            char cmd[MAX_OP_LEN] = "";      \
            s_tolower (#name, cmd);         \
                                            \
            fprintf (output, "%s\n", cmd);  \
            break;                          \
        }

        #include "operations.h"

        #undef DEF_OP

        default:
        {
            fprintf (output, "Error: unknown operation");
            break;
        }
    }
}

void print_var_val (Node *node, FILE *output, Stack *block_names)
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

    fprintf (output, "push ");
    print_var_idx (i, block_names, output, node->value.var);
    //fprintf (output, "push [%d] /*%s*/\n", i, node->value.var);
    //fprintf (output, "dup\n");
    //fprintf (output, "pop [%d] /*%s*/\n", i, node->value.var);
}

void print_call (Node *node, FILE *output, Stack *block_names)
{
    print_arg (node->right, output, block_names);

    int func_idx = find_stk_var(node->value.var, global_stk);

    if (func_idx == 0)
    {
        fprintf (stderr, "Error: func '%s' was not inited.\n", node->value.var);
        return;
    }
    else if (func_idx == L_POISON)
    {
        fprintf (stderr, "Error: func '%s' was multiple inited.\n", node->value.var);
        return;
    }

    fprintf (output, "push %d\n"
                     "push RAX\n"
                     "add\n"
                     "pop RAX\n\n"
                     "call %s\n\n"
                     "push RAX\n"
                     "push %d\n"
                     "sub\n"
                     "pop RAX\n", char_index - 1, node->value.var, char_index - 1);
}

void print_arg (Node *node, FILE *output, Stack *block_names)
{
    if (!(node))
    {
        return;
    }

    if (node->left)
    {
        print_expr (node->left, output, block_names);
    }
    if (node->right)
    {
        print_arg (node->right, output, block_names);
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

    fprintf (output, "pop [%d + RAX] /*%s*/\n", i, node->value.var);//i?
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

void print_var (Node *node, FILE *output, Stack *block_names)
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

    fprintf (output, "push");
    print_var_idx (i, block_names, output, node->value.var);
    //fprintf (output, "push [%d] /*%s*/\n", i, node->value.var);
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
            print_ass (node, output, block_names);
            break;
        }
        case VAR:
        {
            print_var (node, output, block_names);
            break;
        }
        case IF:
        {
            print_if (node, output, block_names);
            break;
        }
        case WHILE:
        {
            print_while (node, output, block_names);
            break;
        }
        case RET:
        {
            print_ret (node, output, block_names);
            break;
        }
        case CALL:
        {
            print_call (node, output, block_names);
            break;
        }
        default:
        {
            debug_print ("Error: unknown action");
            break;
        }
    }
}

void print_ass (Node *node, FILE *output, Stack *block_names)
{
    print_expr (node->right, output, block_names);

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

    fprintf (output, "pop ");
    print_var_idx (i, block_names, output, node->left->value.var);
    //fprintf (output, "pop [%d] /*%s*/\n", i, node->left->value.var);
}

void print_ret (Node *node, FILE *output, Stack *block_names)
{
    print_expr (node->right, output, block_names);

    fprintf (output, "RET\n\n");
}

void print_if (Node *node, FILE *output, Stack *block_names)
{
    static unsigned int if_idx = 0;

    unsigned int cur_if_idx = if_idx;

    fprintf (output, "/*if_%d*/\n\n", cur_if_idx);

    if_idx++;

    print_expr (node->left, output, block_names);

    fprintf (output, "push 0\n"
                     "je if_end_%d\n\n", cur_if_idx, cur_if_idx);

    print_branch (node->right->left, output, block_names);

    fprintf (output, "if_end_%d:\n\n", cur_if_idx);
}

void print_branch (Node *node, FILE *output, Stack *block_names)
{
    if (node && node->type == SEQ)
    {
        print_seq (node->left, output, block_names);
        print_branch (node->right, output, block_names);
    }
}

void print_while (Node *node, FILE *output, Stack *block_names)
{
    static unsigned int while_idx = 0;

    unsigned int cur_while_idx = while_idx;

    fprintf (output, "/*while_start_%d*/\n\nwhile_start_%d:\n", cur_while_idx, cur_while_idx);

    while_idx++;

    print_expr (node->left, output, block_names);

    fprintf (output, "push 0\n"
                     "je while_end_%d\n\n", cur_while_idx, cur_while_idx);

    print_branch (node->right->left, output, block_names);

    fprintf (output, "jmp while_start_%d\n"
                     "while_end_%d:\n\n", cur_while_idx, cur_while_idx);
}

void print_var_idx (int index, Stack *block_names, FILE *output, const char *var)
{
    if (block_names != nullptr && block_names != global_stk)
    {
        fprintf (output, "[%d + RAX]", index);
    }
    else
    {
        fprintf (output, "[%d]", index);
    }

    if (var)
    {
        fprintf (output, " /*%s*/\n", var);
    }
    else
    {
        fprintf (output, "\n");
    }
}

void s_tolower (const char *src, char *dest)
{
    while (*dest++ = tolower (*src++))
        ;
}
