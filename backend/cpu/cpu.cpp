#include <stdio.h>
#include <assert.h>
#include <math.h>

#define STACK_DEBUG
#define PROT_LEVEL 3

#define STACK stk
#define FUNC func

#define POP stack_pop (&(STACK))
#define PUSH(val) stack_push (&(STACK), (val))

#define CPU cpu
#define IP cpu->ip
#define CODE cpu->op_code

#include "cpu.h"
#include "../../../Stack/stack/stack.h"
#include "../calc.h"
#include "cmath"

void process_call_arg (Stack *func, Cpu *cpu);
void process_ret_arg (Stack *func, Cpu *cpu);

int calc (Cpu *cpu, const int n_instr)
{
    assert (cpu && n_instr);
    assert (cpu->op_code);

    int val_1 = 0;
    int val_2 = 0;

    Stack stk = {};
    Stack func = {};

    int start_capacity = 5;

    stack_init (&stk, start_capacity);
    stack_init (&func, start_capacity);

    while (cpu->ip < n_instr)
    {
        int cmd = CODE[(IP)++];
        int arg = 0;

        switch ((char)cmd & CMD_MASK)
        {
            #define DEF_CMD(name, num, arg, arg_name, code) \
            case num:                                       \
            {                                               \
                code                                        \
                break;                                      \
            }
            #define DEF_JMP(name, num, arg, arg_name, sign) \
            case num:                                       \
            {                                               \
                val_1 = POP;                                \
                val_2 = POP;                                \
                                                            \
                                                            \
                if (val_2 sign val_1)                       \
                {                                           \
                    IP = CODE[IP];                          \
                }                                           \
                else                                        \
                {                                           \
                    (IP)++;                                 \
                }                                           \
                                                            \
                break;                                      \
            }

            #include "../cmd.h"

            #undef DEF_CMD
            #undef DEF_JMP

            default:
            {
                fprintf (stderr, "ERROR: incorrect input info [%d][%d]\n", CODE[IP], IP);
                return 0;
            }
        }
    }

    return true;
}


int process_push_arg (int cmd, Cpu *cpu)
{
    assert (cpu);
    assert (cpu->op_code);

    int arg = 0;

    if (cmd & ARG_IMMED)
    {
        arg += cpu->op_code[(cpu->ip)++];
    }
    if (cmd & ARG_REGISTR)
    {
        arg += cpu->regs[cpu->op_code[(cpu->ip)++]];
    }
    if (cmd & ARG_MEM)
    {
        arg = cpu->RAM[arg];
    }

    return arg;
}

void process_pop_arg (int cmd, Cpu *cpu, int arg)
{
    assert (cpu);
    assert (cpu->op_code);

    if ((cmd & ARG_MEM) > 0)
    {
        if ((cmd & ARG_IMMED) > 0 && (cmd & ARG_REGISTR) > 0)
        {
            cpu->RAM[cpu->op_code[cpu->ip] + cpu->regs[cpu->op_code[cpu->ip + 1]]] = arg;
            cpu->ip += 2;
        }
        else if (cmd & ARG_IMMED)
        {
            cpu->RAM[cpu->op_code[(cpu->ip)++]] = arg;
        }
        else if (cmd & ARG_REGISTR)
        {
            cpu->RAM[cpu->regs[cpu->op_code[(cpu->ip)++]]] = arg;
        }
    }
    else if ((cmd & ARG_REGISTR) > 0)
    {
        cpu->regs[cpu->op_code[(cpu->ip)++]] = arg;
    }
}

void process_call_arg (Stack *func, Cpu *cpu)
{
    assert (cpu && func);
    assert (cpu->op_code);

    int a = (cpu->ip) + 1;
    stack_push (func, a);

    int call_ip = cpu->op_code[cpu->ip];
    cpu->ip = call_ip;
}

void process_ret_arg (Stack *func, Cpu *cpu)
{
    int a = stack_pop (func);
    cpu->ip = a;
}
