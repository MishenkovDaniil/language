#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "input.h"
#include "lexems.h"
#include "tree.h"

#include "../../standart_functions/io/io.h"

int main (int argc, const char **argv)
{
    if (argc != 3)
    {
        fprintf (stderr, "Error: too few arguments.\n");
        return 0;
    }

    FILE *exe_file = fopen (argv[1], "r");
    FILE *tree_file = fopen (argv[2], "w");

    assert (exe_file);
    assert (tree_file);

    int txt_size = get_file_size (argv[1]);
    int n_lines = 0;
    int verbose = 1;

    char *text = (char *)calloc (txt_size + 2, sizeof (char));
    assert (text);

    n_lines = read_in_buf (exe_file, text, &n_lines, txt_size, verbose);

    Lexem *lexems[100] = {};

    lexer (text, lexems);

    print_lexems (lexems, tree_file);

    Tree tree = {};
    unsigned int err =0;

    tree_fill (&tree, lexems);
    fprintf (stderr, "root->left [%p]\n", tree.root->left);
    tree_check (&tree, &err);


    tree_dtor (&tree);
    free_lexems (lexems);

    free (text);
}
