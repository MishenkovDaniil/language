#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "../../../standart_functions/io/io.h"
#include "asm.h"
#include "../binary.h"
#include "../labels.h"

int main (int argc, const char *argv[])
{
    if (argc != 3)
    {
        printf ("Enter name of the object file, name of input and output files to assembly and nothing more");

        return 0;
    }

    int nlines = 0;
    int verbose = 1;    //debug

    FILE *asm_file = fopen (argv[1], "rb");
    FILE *out_file = fopen (argv[2], "wb");

    assert (asm_file);
    assert (out_file);

    int txt_size = get_file_size (argv[1]);

    char *text = (char *)calloc (txt_size + 2, sizeof (char));
    assert (text);

    nlines = read_in_buf (asm_file, text, &nlines, txt_size, verbose);

    int *opcode = (int *)calloc (MAX_OP_LEN * nlines, sizeof (int));
    assert (opcode);

    Label label = {};

    init_code (text, opcode, &label);
    int n_instr = init_code (text, opcode, &label);

    write_opcode (out_file, opcode, n_instr, MAGIC_NUMBER, VERSION);

    free (text);
    free (opcode);

    fclose (asm_file);
    fclose (out_file);

    return 0;
}
