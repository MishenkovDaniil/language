#include <stdio.h>
#include <assert.h>

#include "input.h"

const char *s = nullptr;

int GetG (const char *str)
{
    s = str;

    int value = GetE ();

    assert (*s == '\0');

    return value;
}

int GetE (void)
{
    int value = GetT ();

    while (*s == '+' || *s == '-')
    {
        const char op = *s;

        s++;

        int val_2 = GetT ();

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

int GetT (void)
{
    int value = GetP ();

    while (*s == '*' || *s == '/')
    {
        const char op = *s;

        s++;

        int val_2 = GetP ();

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

int GetP (void)
{
    int value = 0;

    if (*s == '(')
    {
        s++;

        value = GetE ();

        assert (*s == ')');

        s++;
    }
    else
    {
        value = GetN ();
    }

    return value;
}


int GetN (void)
{
    int value = 0;

    const char *sOld = s;

    while ('0' <= *s && *s<= '9')
    {
        value = value*10 + *s - '0';
        s++;
    }

    assert (s != sOld);

    return value;
}
