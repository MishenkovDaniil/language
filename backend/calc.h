#ifndef CALC_H
#define CALC_H

static const int RAM_SIZE = 100;
static const int REG_SIZE = 5;
static const int CMD_MASK = 0X1F;

enum op_codes
{
    #define DEF_JMP(name, num,...) DEF_CMD(name, num, __VA_ARGS__)
    #define DEF_CMD(name, num,...) CMD_##name = num,

    #include "cmd.h"

    #undef DEF_CMD
    #undef DEF_JMP
};

enum calc_errors
{
    SYNTAX_ERROR = 0x1 << 0,
    ALLOC_FAIL   = 0x1 << 1,
    FOPEN_FAIL   = 0x1 << 2
};

enum types
{
    ARG_IMMED = 0x20,
    ARG_REGISTR = 0x40,
    ARG_MEM = 0x80
};

enum regs
{
    #define DEF_REG(name, num) name##_NUM = num,

    #include "regs.h"

    #undef DEF_REG
};

struct Cpu
{
    int *op_code = nullptr;

    int ip = 0;

    int regs[REG_SIZE] = {0};

    int RAM[RAM_SIZE] = {0};
};

#endif /* CALC_H */

