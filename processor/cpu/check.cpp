#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cpu.h"
#include "../calc.h"
#include "../binary.h"

int check_asm_file (Header *header, const int magic_number, const int version)
{
    assert (header);
    assert (magic_number);

    if (header->magic_number != magic_number)
    {
        fprintf (stderr, "ERROR: file_type is incorrect for this CPU");

        return CPU_INCORRECT_ID;
    }
    if (header->file_version != version)
    {
        fprintf (stderr, "ERROR: file version is not suitable for this CPU");

        return CPU_INCORRECT_VERSION;
    }
    if (header->n_instr < 3)
    {
        fprintf (stderr, "ERROR: number of commands is too low");

        return CPU_INCORRECT_NUM;
    }

    return 0;
}
