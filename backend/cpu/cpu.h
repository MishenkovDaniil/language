#ifndef CPU_H
#define CPU_H

#include "../calc.h"
#include "../binary.h"

int check_asm_file (Header *header, const int magic_number, const int version);
int calc (Cpu *calc, const int n_instr);
int process_push_arg (int cmd, Cpu *cpu);
void process_pop_arg (int cmd, Cpu *cpu, int arg);

enum code_errors
{
    CPU_INCORRECT_ID      = 0x1 << 0,
    CPU_INCORRECT_VERSION = 0x1 << 1,
    CPU_INCORRECT_NUM     = 0x1 << 2
};

#endif /* CPU_H */
