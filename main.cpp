#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "io/input.h"
#include "../standart_functions/io/io.h"

int main ()
{
    FILE *input_file = fopen ("expression.txt", "r");
    int expr_size = get_file_size ("expression.txt");

    char *expr = (char *)calloc (expr_size, sizeof(char));
    fread (expr, sizeof(char), expr_size, input_file);

    printf ("[%s]", expr);

    int value = GetG (expr);

    printf ("%d", value);

}

