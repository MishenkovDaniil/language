#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../calc.h"
#include "../binary.h"
#include "../labels.h"
#include "disasm.h"

void diasm_code (const char *disasm, Label *labels, Header header, int *instrs)
{
    FILE *disasm_file = fopen (disasm, "wb");

    int index = 0;
    int label_index = 0;
    int jmp_index = 0;

    while (index < header.n_instr)
    {
        int cmd = instrs[index++];

        if (find_label (labels, index - 1))
        {
            fprintf (disasm_file, "LABEL_%d:\n", index - 1);
        }

        switch ((char)cmd & CMD_MASK)
        {
        #define DEF_JMP(name, num, arg, process_name,...)                                               \
                DEF_CMD(name, num, arg, process_name, __VA_ARGS__)
        #define DEF_CMD(name, num, arg, process_name,...)                                               \
        case num:                                                                                       \
        {                                                                                               \
            fprintf (disasm_file, #name" ");                                                            \
            if (arg)                                                                                    \
            {                                                                                           \
                process_##process_name##_arg (disasm_file, labels, cmd, instrs, &index, &label_index); \
            }                                                                                           \
            else                                                                                        \
            {                                                                                           \
                fprintf (disasm_file, "\n");                                                            \
            }                                                                                           \
                                                                                                        \
            break;                                                                                      \
        }
        #include "..\cmd.h"

        #undef DEF_REG
        #undef DEF_CMD

        default:
        {
            printf ("ERROR: command is not readable");
        }
        }
    }

    fclose (disasm_file);
}

void process_push_arg (FILE *disasm_file, Label *labels, int cmd, int *instrs, int *index, int *label_index)
{
    if (cmd & ARG_MEM)
    {
        fprintf (disasm_file, "[ ");
    }
    if (cmd & ARG_IMMED)
    {
        fprintf (disasm_file, "%d ", instrs[(*index)++]);
    }
    if (cmd & ARG_REGISTR)
    {
        if (cmd & ARG_IMMED)
        {
            fprintf (disasm_file, "+ ");
        }
        #define DEF_REG(name, num)\
        if (instrs[*index] == num)\
        {\
            fprintf (disasm_file, #name" ");\
        }\
        else

        #include "../regs.h"

        #undef DEF_REG
        /*else*/
        {
            printf ("ERROR: push arg is not readable");
        }
    }
    if (cmd & ARG_MEM)
    {
        fprintf (disasm_file, "]");
    }
    fprintf (disasm_file, "\n");
}

void process_jmp_arg (FILE *disasm_file, Label *labels, int cmd, int *instrs, int *index, int *label_index)
{
    int label_instr = instrs[(*index)++];

    fprintf (disasm_file, "label_%d\n", label_instr);

    if (!(find_label (labels, label_instr)))
    {
        labels->value[(*label_index)++] = label_instr;
    }
}

int find_label (Label *labels, int value)
{
    for (int index = 0; index < MAXSIZE; index++)
    {
        if (labels->value[index] == value)
        {
            return 1;
        }
    }
    return 0;
}
