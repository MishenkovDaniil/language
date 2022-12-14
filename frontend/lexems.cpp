#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexems.h"
#include "input.h"
#include "stack.h"

void skip_spaces (char **str)
{
    while (isspace (**str))
    {
        (*str)++;
    }
}

Lexem **lexer (char *text, Stack *lexems)
{
    int shift = 0;
    char *cur_smbl = text;
    bool is_needed = true;

    int idx = 0 ;

    while (*(cur_smbl))
    {
        is_needed  = true;

        if (isspace (*(cur_smbl)))
        {
            skip_spaces (&cur_smbl);
            continue;
        }

        Lexem *lexem = (Lexem *)calloc (1, sizeof (Lexem));

        bool is_char = true;

        switch (*(cur_smbl++))
        {
            case '(':
            {
                lexem->type = L_STARTING_BRACKET;
                break;
            }
            case ')':
            {
                lexem->type = L_CLOSING_BRACKET;
                break;
            }
            case '{':
            {
                lexem->type = L_BLOCK_START;
                break;
            }
            case '}':
            {
                lexem->type = L_BLOCK_END;
                break;
            }
            case '+':
            {
                lexem->type = L_OP;
                lexem->value.op_val = ADD;
                break;
            }
            case '-':
            {
                lexem->type = L_OP;
                lexem->value.op_val = SUB;
                break;
            }
            case '*':
            {
                lexem->type = L_OP;
                lexem->value.op_val = MUL;
                break;
            }
            case '/':
            {
                lexem->type = L_OP;
                lexem->value.op_val = DIV;
                break;
            }
            case '^':
            {
                lexem->type = L_OP;
                lexem->value.op_val = DEG;
                break;
            }
            case ';':
            {
                lexem->type = L_SEQ;
                break;
            }
            case '=':
            {
                if (lexems->size > 0) //lexems[idx - 1]->type == L_VAR)
                {
                    Lexem *pop_lexem = (Lexem *)stack_pop (lexems);
                    if (pop_lexem->type == L_VAR)
                    {
                        stack_push (lexems, pop_lexem);
                        lexem->type = L_ASS;
                    }
                    else if (pop_lexem->type == L_NVAR)
                    {
                        stack_push (lexems, pop_lexem);
                        is_needed = false;
                    }
                }
                else
                {
                    fprintf (stderr, "Error: '=' not after variable value.\n");
                }
                /* if (lexems[idx-1]->type == L_NVAR)
                {
                    is_needed = false;
                }*/

                break;
            }
            case ',':
            {
                is_needed = false;
                //lexem->type = L_SEP;
                break;
            }
            default:
            {
                is_char = false;
                cur_smbl--;
                break;
            }
        }

        if (is_char)
        {
            0;
        }
        else if (isdigit(*(cur_smbl)))
        {
            char *end = nullptr;
            double val = strtod (cur_smbl, &end);

            lexem->type = L_NUM;
            lexem->value.dbl_val = val;

            cur_smbl = end;
        }
        else if (strstr (cur_smbl, "if") == cur_smbl)
        {
            cur_smbl += strlen ("if");
            lexem->type = L_IF;
        }
        else if (strstr (cur_smbl, "while") == cur_smbl)
        {
            cur_smbl += strlen ("while");
            lexem->type = L_WHILE;
        }
        else if (strstr (cur_smbl, "CALL") == cur_smbl)
        {
            cur_smbl += strlen ("call");

            skip_spaces (&cur_smbl);

            if (!(isalpha (*(cur_smbl))))
            {
                debug_print ("wrong variable name, %s", cur_smbl);
                return nullptr;
            }

            char var[10] = "";
            int num = 0;

            sscanf (cur_smbl, "%[a-z A-Z _ 1-9]%n", var, &num);
            cur_smbl += num;

            lexem->type = L_CALL;
            lexem->value.var = (char *)calloc (strlen(var) + 1, sizeof (char));
            strcpy (lexem->value.var, var);
        }
        else if (strstr (cur_smbl, "return") == cur_smbl)
        {
            cur_smbl += strlen ("return");
            lexem->type = L_RET;
        }
        else if (strstr (cur_smbl, "NVAR") == cur_smbl)
        {
            cur_smbl += strlen ("NVAR") + 1;

            skip_spaces (&cur_smbl);

            if (!(isalpha (*(cur_smbl))))
            {
                debug_print ("wrong variable name, %s", cur_smbl);
                return nullptr;
            }

            char var[10] = "";
            int num = 0;

            sscanf (cur_smbl, "%[a-z A-Z _ 1-9]%n", var, &num);
            cur_smbl += num;

            lexem->type = L_NVAR;
            lexem->value.var = (char *)calloc (strlen(var) + 1, sizeof (char));
            strcpy (lexem->value.var, var);
        }
        else if (strstr (cur_smbl, "NFUNC") == cur_smbl)
        {
            cur_smbl += strlen ("NFUNC") + 1;

            skip_spaces (&cur_smbl);

            if (!(isalpha (*(cur_smbl))))
            {
                debug_print ("wrong func name, %s", cur_smbl);
                return nullptr;
            }

            char var[10] = "";
            int num = 0;

            sscanf (cur_smbl, "%[a-z A-Z _ 1-9]%n", var, &num);
            cur_smbl += num;

            lexem->type = L_NFUN;
            lexem->value.var = (char *)calloc (strlen(var) + 1, sizeof (char));
            strcpy (lexem->value.var, var);
        }/*
        else if (strstr (cur_smbl, "DEF") == cur_smbl)
        {
            cur_smbl += strlen ("DEF");
            lexem->type = L_DEF;
        }*/
        else if (isalpha (*cur_smbl))
        {
            char var[10] = "";
            int num = 0;

            sscanf (cur_smbl, "%[a-z A-Z _ 1-9]%n", var, &num);
            cur_smbl += num;

            lexem->type = L_VAR;
            lexem->value.var = (char *)calloc (strlen(var) + 1, sizeof (char));
            strcpy (lexem->value.var, var);
        }
        else
        {
            fprintf (stderr, "Error:{%s}.\n", cur_smbl);
            return nullptr;
        }

        if (is_needed)
        {
            stack_push (lexems, lexem);
            stack_push (lexems, lexem);
            fprintf (stderr, "[%d]\n", ((Lexem *)stack_pop(lexems))->type);
        }
    }

    return nullptr;
}

void print_lexems (Stack *lexems, FILE *output_file)
{
    if (!(lexems->size))
    {
        return;
    }

    Lexem *pop_lexem = (Lexem *)stack_pop (lexems);
    print_lexems (lexems, output_file);
    stack_push (lexems, pop_lexem);

    switch (pop_lexem->type)
    {
        case L_DEFAULT:
        {
            fprintf (output_file, "[DEFAULT]\n");
            break;
        }
        case L_NUM:
        {
            fprintf (output_file, "[NUM, %.2lf]\n", pop_lexem->value.dbl_val);
            break;
        }
        case L_VAR:
        {
            fprintf (output_file, "[VAR, %s]\n", pop_lexem->value.var);
            break;
        }
        case L_OP:
        {
            fprintf (output_file, "[OP, %d]\n", pop_lexem->value.op_val);
            break;
        }
        case L_CALL:
        {
            fprintf (output_file, "[CALL, %s]\n", pop_lexem->value.var);
            break;
        }
        case L_IF:
        {
            fprintf (output_file, "[IF]\n");
            break;
        }
        case L_WHILE:
        {
            fprintf (output_file, "[WHILE]\n");
            break;
        }
        case L_NFUN:
        {
            fprintf (output_file, "[NFUN, %s]\n", pop_lexem->value.var);
            break;
        }
        case L_NVAR:
        {
            fprintf (output_file, "[NVAR, %s]\n", pop_lexem->value.var);
            break;
        }
        case L_SEP:
        {
            fprintf (output_file, "[SEP]\n");
            break;
        }
        case L_PAR:
        {
            fprintf (output_file, "[PAR, %s]\n", pop_lexem->value.var);
            break;
        }
        case L_SEQ:
        {
            fprintf (output_file, "[SEQ]\n");
            break;
        }
        case L_BLOCK_START:
        {
            fprintf (output_file, "[BLOCK_START]\n");
            break;
        }
        case L_BLOCK_END:
        {
            fprintf (output_file, "[BLOCK_END]\n");
            break;
        }
        case L_STARTING_BRACKET:
        {
            fprintf (output_file, "[STARTING_BRACKET]\n");
            break;
        }
        case L_CLOSING_BRACKET:
        {
            fprintf (output_file, "[CLOSING_BRACKET]\n");
            break;
        }
        case L_RET:
        {
            fprintf (output_file, "[RET]\n");
            break;
        }
        case L_ASS:
        {
            fprintf (output_file, "[ASS]\n");
            break;
        }
        default:
        {
            debug_print ("Error: wrong command %d.\n", pop_lexem->type);
            break;
        }
    }
    fflush (output_file);
}

void free_lexems (Lexem **lexems)
{
    for (int i = 0; lexems[i] != nullptr; i++)
    {
        /*if (lexems[i]->type == L_VAR)
        {
            free (lexems[i]->value.var);
        }*/

        free (lexems[i]);
    }
}
