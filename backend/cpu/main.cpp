#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "cpu.h"
#include "../calc.h"
#include "../binary.h"

int main (int argc, const char *argv[])
{
    if (argc > 6)
    {
        printf ("ERROR: enter input file, register values (if needed) and nothing more");

        return 0;
    }

    FILE *asm_file = fopen (argv[1], "rb");
    assert (asm_file);
    --argc;

    struct Header header = {};
    struct Cpu cpu = {};

    int reg_number = 1;

    while (--argc > 0)
    {
        cpu.regs[reg_number] = atoi (argv[reg_number + 1]);
        reg_number++;
    }

    fread (&header, sizeof (header), 1, asm_file);

    if (check_asm_file (&header, MAGIC_NUMBER, VERSION))
    {
        return 0;
    }

    cpu.op_code = (int *)calloc (header.n_instr, sizeof (int));
    assert (cpu.op_code);

    fread (cpu.op_code, sizeof (int), header.n_instr, asm_file);

    calc (&cpu, header.n_instr);

    free (cpu.op_code);
    fclose (asm_file);

    return 0;
}

