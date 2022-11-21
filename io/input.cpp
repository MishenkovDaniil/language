#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#include "input.h"

void skip_spaces (const char **str)
{
    while (isspace (**str))
    {
        (*str)++;
    }
}

int GetG (const char *expr)
{
    skip_spaces (&expr);
    int value = GetE (&expr);
    skip_spaces (&expr);

    assert (*expr == '\0');

    return value;
}

int GetE (const char **expr)
{
    skip_spaces (expr);
    int value = GetT (expr);
    skip_spaces (expr);

    while (**expr == '+' || **expr == '-')
    {
        const char op = **expr;

        (*expr)++;

        int val_2 = GetT (expr);

        if (op == '+')
        {
            value += val_2;
        }
        else
        {
            value -= val_2;
        }
    }

    return value;
}

int GetT (const char **expr)
{
    skip_spaces (expr);
    int value = GetP (expr);
    skip_spaces (expr);

    while (**expr == '*' || **expr == '/')
    {
        const char op = **expr;

        (*expr)++;

        int val_2 = GetP (expr);

        if (op == '*')
        {
            value *= val_2;
        }
        else
        {
            value /= val_2;
        }
    }

    return value;
}

int GetP (const char **expr)
{
    skip_spaces (expr);

    int value = 0;

    if (**expr == '(')
    {
        (*expr)++;

        value = GetE (expr);

        skip_spaces (expr);

        assert (**expr == ')');

        (*expr)++;
    }
    else
    {
        value = GetN (expr);
    }

    return value;
}


int GetN (const char **expr)
{
    skip_spaces (expr);

    int value = 0;

    const char *exprOld = *expr;

    while ('0' <= **expr && **expr <= '9')
    {
        value = value*10 + **expr - '0';
        (*expr)++;
    }

    assert (*expr != exprOld);

    return value;
}
