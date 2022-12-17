#ifndef LEXEMS_H
#define LEXEMS_H

enum Lexem_types
{
    DEFAULT = 0,

    NUM     = 1,
    VAR     = 2,
    OP      = 3,

    CALL    = 4,

    IF      = 5,
    WHILE   = 6,

    DEF     = 7,
    NVAR    = 8,

    PAR     = 10,
    SEQ     = 11,

    BLOCK_START = 12,
    BLOCK_END   = 13,

    STARTING_BRACKET = 14,
    CLOSING_BRACKET  = 15,

    RET = 16,
    SEP = 17,
};

enum Op_types
{
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    DEG = 5,

    ARG = 6,

    AND = 7,
    OR  = 8,
    NOT = 9,

    ASS = 10,
};

union Value
{
    double dbl_val;
    char *var;
    Op_types op_val;
};

struct Lexem
{
    Lexem_types type = DEFAULT;
    Value value = {};
};

#endif /* LEXEMS_H */
