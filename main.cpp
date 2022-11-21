#include <stdio.h>
#include <assert.h>

#include "input.h"

int main ()
{
    /*FILE *input_file = fopen ("expression.txt"), "r");
    char *expression = */

    int value = GetG ("(2+3*5)*3-5");

    printf ("%d", value);

}

