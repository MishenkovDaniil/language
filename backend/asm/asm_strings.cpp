#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "asm_strings.h"

int is_empty_str (char *arr)
{
    while (*arr != '\0')
    {
        if (!(isspace (*arr)))
        {
            return 0;
        }
        else
        {
            arr++;
        }
    }
    return 1;
}

void skip_spaces (char **text)
{
    while (isspace (**text))
    {
        (*text)++;
    }
}

int is_comment (char *cmd, char **text)
{
    int return_value = 0;

    if (strstr (cmd, "//"))
    {
        if (!(strchr (cmd, '\n')))
        {
            *text = strchr (*text, '\n');
            skip_spaces (text);
        }

        return_value = 1;
    }
    if (strstr (cmd, "/*"))
    {
        char *comm_end = strstr (*text, "*/");

        if (strstr (cmd, "*/"))
        {
            return_value = 1;
        }
        else if (comm_end)
        {
            *text = comm_end;
            *text += 2;

            skip_spaces (text);

            return_value = 1;
        }
        else
        {
            printf ("Error: not closed comment {%s}.\n", cmd);
            return -1;
        }
    }

    return return_value;
}
