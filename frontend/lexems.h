#ifndef LEXEMS_H
#define LEXEMS_H

#include "op_types.h"

enum Lexem_types
{
    L_DEFAULT = 0,

    L_NUM     = 1,
    L_VAR     = 2,
    L_OP      = 3,

    L_CALL    = 4,

    L_IF      = 5,
    L_WHILE   = 6,

    L_NFUN     = 7,
    L_NVAR    = 8,

    L_PAR     = 10,
    L_SEQ     = 11,
    L_BLOCK_START = 12,
    L_BLOCK_END   = 13,

    L_STARTING_BRACKET = 14,
    L_CLOSING_BRACKET  = 15,

    L_RET = 16,
    L_SEP = 17,
    L_ASS = 18,
    L_ARG = 19,
};


union Val
{
    double dbl_val;
    char *var;
    Op_types op_val;
};

struct Lexem
{
    Lexem_types type = L_DEFAULT;
    Val value = {};
};


#endif /* LEXEMS_H */
