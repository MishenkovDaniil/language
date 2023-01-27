#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../../../standart_functions/io/io.h"
#include "../calc.h"
#include "../binary.h"
#include "../labels.h"
#include "disasm.h"

void process_push_arg (FILE *disasm_file, int cmd, int *instrs, int *index);
void process_jmp_arg (FILE *disasm_file, int cmd, int *instrs, int *index);


int main (int argc, const char **argv)
{
    if (argc != 3)
    {
        printf ("enter file to disassembly and output file");
    }

    FILE *asm_file = fopen (argv[1], "rb");

    assert (asm_file);//error

    struct Header header = {};
    fread (&header, sizeof (header), 1, asm_file);

    int *instrs = (int *)calloc (header.n_instr, sizeof (int));

    assert (instrs);//error

    fread (instrs, sizeof (int), header.n_instr, asm_file);

    Label labels = {};

    diasm_code (argv[2], &labels, header, instrs);
    diasm_code (argv[2], &labels, header, instrs);

    FILE *disasm_file = fopen (argv[2], "a");
    assert (disasm_file); // error

    fprintf (disasm_file, "magic_number is %d\nversion is %d\nnumber of instr is %d\n", header.magic_number, header.file_version, header.n_instr);

    fclose  (disasm_file);

    free (instrs);

    return 0;
}
