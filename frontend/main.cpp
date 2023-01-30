#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "input.h"
#include "lexems.h"
#include "tree.h"
#include "output.h"
#include "stack.h"
#include "check_input.h"


int main (int argc, const char **argv)
{
    unsigned int err = 0;

    if (!(check_argc (argc, argv)))
    {
        return 0;
    }

    FILE *exe_file  = fopen (argv[1], "r");
    FILE *tree_file = fopen (argv[2], "w");
    FILE *asm_file  = fopen (argv[3], "w");

    if (!(check_files (3, exe_file, argv[1], tree_file, argv[2], asm_file, argv[3])))
    {
        return 0;
    }

    Stack lexems = {};

    make_lexems (argv[1], exe_file, &lexems);
    print_lexems (&lexems, tree_file);

    Tree tree = {};

    tree_fill (&tree, &lexems);
    tree_check (&tree, &err);

    print_tree (&tree, asm_file);

    stack_dtor (&lexems);
    tree_dtor (&tree);
    //free_lexems (lexems);

    return 0;
}
