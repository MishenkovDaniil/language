#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "../../standart_functions/io/io.h"
#include "check_input.h"

bool check_argc (int argc, const char **argv)
{
    if (argc != 4)
    {
        fprintf (stderr, "Error: incorrect number of arguments.\n");
        printf ("Enter programm file, tree file and out file.\n");

        return false;
    }

    return true;
}

bool check_files (int num_of_files,...)
{
    va_list fileinf;
    va_start (fileinf, num_of_files);

    for (int i = 1; i < num_of_files; i++)
    {
        FILE *file = va_arg (fileinf, FILE *);
        const char *filename = va_arg (fileinf, const char *);

        if (!(check_file (file, filename)))
        {
            return false;
        }
    }

    return true;
}

bool check_file (FILE *file, const char *filename)
{
    if (file == nullptr)
    {
        fprintf (stderr, "Error: cannot open file %s.\n", filename);

        return false;
    }

    assert (file);

    return true;
}
