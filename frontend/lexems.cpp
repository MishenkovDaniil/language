#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "syntax.h"
#include "input.h"

void skip_spaces (char **str)
{
    while (isspace (**str))
    {
        (*str)++;
    }
}

Lexem **lexer (char *text, Lexem **lexems)
{
    int shift = 0;
    char *cur_smbl = text;

    int idx = 0 ;

    while (*(cur_smbl))
    {
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
                lexem->type = STARTING_BRACKET;
                break;
            }
            case ')':
            {
                lexem->type = STARTING_BRACKET;
                break;
            }
            case '{':
            {
                lexem->type = BLOCK_START;
                break;
            }
            case '}':
            {
                lexem->type = BLOCK_END;
                break;
            }
            case '+':
            {
                lexem->type = OP;
                lexem->value.op_val = ADD;
                break;
            }
            case '-':
            {
                lexem->type = OP;
                lexem->value.op_val = SUB;
                break;
            }
            case '*':
            {
                lexem->type = OP;
                lexem->value.op_val = MUL;
                break;
            }
            case '/':
            {
                lexem->type = OP;
                lexem->value.op_val = DIV;
                break;
            }
            case '^':
            {
                lexem->type = OP;
                lexem->value.op_val = DEG;
                break;
            }
            case ';':
            {
                lexem->type = SEQ;
                break;
            }
            case '=':
            {
                lexem->type = OP;
                lexem->value.op_val = ASS;
                break;
            }
            case ',':
            {
                lexem->type = SEP;
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

            lexem->type = NUM;
            lexem->value.dbl_val = val;

            cur_smbl = end;
        }
        else if (strstr (cur_smbl, "if") == cur_smbl)
        {
            cur_smbl += strlen ("if");
            lexem->type = IF;
        }
        else if (strstr (cur_smbl, "while") == cur_smbl)
        {
            cur_smbl += strlen ("while");
            lexem->type = WHILE;
        }
        else if (strstr (cur_smbl, "return") == cur_smbl)
        {
            cur_smbl += strlen ("return");
            lexem->type = RET;
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

            lexem->type = NVAR;
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

            lexem->type = DEF;
            lexem->value.var = (char *)calloc (strlen(var) + 1, sizeof (char));
            strcpy (lexem->value.var, var);
        }
        else if (strstr (cur_smbl, "DEF") == cur_smbl)
        {
            cur_smbl += strlen ("DEF");
            lexem->type = DEF;
        }
        else if (isalpha (*cur_smbl))
        {
            char var[10] = "";
            int num = 0;

            sscanf (cur_smbl, "%[a-z A-Z _ 1-9]%n", var, &num);
            cur_smbl += num;

            lexem->type = VAR;
            lexem->value.var = (char *)calloc (strlen(var) + 1, sizeof (char));
            strcpy (lexem->value.var, var);
        }
        else
        {
            fprintf (stderr, "Error:{%s}.\n", cur_smbl);
            return nullptr;
        }

        lexems[idx++] = lexem;
        fprintf (stderr, "[%d]\n", lexems[idx-1]->type);
    }

    return lexems;
}

void print_lexems (Lexem **lexems, FILE *output_file)
{
    for (int i = 0; lexems[i] != nullptr; i++)
    {
        switch (lexems[i]->type)
        {
            case DEFAULT:
            {
                fprintf (output_file, "[DEFAULT]\n");
                break;
            }
            case NUM:
            {
                fprintf (output_file, "[NUM]\n");
                break;
            }
            case VAR:
            {
                fprintf (output_file, "[VAR, %s]\n", lexems[i]->value.var);
                break;
            }
            case OP:
            {
                fprintf (output_file, "[OP, %d]\n", lexems[i]->value.op_val);
                break;
            }
            case CALL:
            {
                fprintf (output_file, "[CALL]\n");
                break;
            }
            case IF:
            {
                fprintf (output_file, "[IF]\n");
                break;
            }
            case WHILE:
            {
                fprintf (output_file, "[WHILE]\n");
                break;
            }
            case DEF:
            {
                fprintf (output_file, "[DEF]\n");
                break;
            }
            case NVAR:
            {
                fprintf (output_file, "[NVAR, %s]\n", lexems[i]->value.var);
                break;
            }
            case SEP:
            {
                fprintf (output_file, "[SEP]\n");
                break;
            }
            case PAR:
            {
                fprintf (output_file, "[PAR, %s]\n", lexems[i]->value.var);
                break;
            }
            case SEQ:
            {
                fprintf (output_file, "[SEQ]\n");
                break;
            }
            case BLOCK_START:
            {
                fprintf (output_file, "[BLOCK_START]\n");
                break;
            }
            case BLOCK_END:
            {
                fprintf (output_file, "[BLOCK_END]\n");
                break;
            }
            case STARTING_BRACKET:
            {
                fprintf (output_file, "[STARTING_BRACKET]\n");
                break;
            }
            case CLOSING_BRACKET:
            {
                fprintf (output_file, "[CLOSING_BRACKET]\n");
                break;
            }
            case RET:
            {
                fprintf (output_file, "[RET]\n");
                break;
            }
            default:
            {
                debug_print ("Error: wrong command.\n");
                break;
            }
        }
    }
}

void free_lexems (Lexem **lexems)
{
    for (int i = 0; lexems[i] != nullptr; i++)
    {
        free (lexems[i]);
    }
}
