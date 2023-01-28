DEF_CMD (HLT,  0, 0, push,
{
    stack_dtor (&(STACK));
    stack_dtor (&(FUNC));
    cpu->ip = n_instr;
})

DEF_CMD (PUSH, 1, 1, push,
{
    arg = process_push_arg (cmd, cpu);
    stack_push (&(STACK), arg);
})

DEF_CMD (POP,  2, 1, push,
{
    arg = stack_pop (&(STACK));
    process_pop_arg (cmd, cpu, arg);
})

DEF_CMD (SUB,  3, 0, push,
{
    val_1  = stack_pop (&(STACK));
    stack_push (&(STACK), (POP) - val_1);
})

DEF_CMD (ADD,  4, 0, push,
{
    stack_push (&(STACK), (POP) + (POP));
})

DEF_CMD (MULT, 5, 0, push,
{
    stack_push (&(STACK), (POP) * (POP));
})

DEF_CMD (DIV,  6, 0, push,
{
    val_1 = POP;

    PUSH ((int)round((float)((100 * POP) / val_1) / 100.0));
})

DEF_CMD (OUT, 7, 0, push,
{
    val_1 = POP;
    fprintf (stderr, "\nresult is [%d]\n", val_1);
})

DEF_CMD (DUP, 8, 0, push,
{
    val_1 = POP;
    PUSH (val_1);
    PUSH (val_1);
})

DEF_CMD (CALL, 9, 1, jmp,
{
    process_call_arg (&func, cpu);
})

DEF_CMD (RET, 10, 0, push,
{
    process_ret_arg (&func, cpu);
})

DEF_CMD (JMP, 11, 1, jmp,
{
    IP = CODE[IP];
})

DEF_JMP (JB,  12, 1, jmp, <)
DEF_JMP (JBE, 13, 1, jmp, <=)
DEF_JMP (JA,  14, 1, jmp, >)
DEF_JMP (JAE, 15, 1, jmp, >=)
DEF_JMP (JE,  16, 1, jmp, ==)
DEF_JMP (JNE, 17, 1, jmp, !=)
DEF_JMP (JT,  18, 1, jmp, ==)

DEF_CMD (IN,  19, 0, push,
{
    int af = 0;
    scanf ("%d", &af);
    PUSH (af);
})

DEF_CMD(SQRT, 20, 0, push,
{
    int ae = 10000 * POP;
    double be = sqrt (ae);

    PUSH((int)round((float)be / 100.0));
})

DEF_CMD (AND, 21, 0, push,
{
    val_1 = POP;

    stack_push (&(STACK), (POP) && (val_1));
})

DEF_CMD (OR, 22, 0, push,
{
    stack_push (&(STACK), (POP) || (POP));
})

DEF_CMD (LT, 23, 0, push,
{
    val_1 = POP;

    stack_push (&(STACK), (POP) < val_1);
})
DEF_CMD (GT, 24, 0, push,
{
    val_1 = POP;

    stack_push (&(STACK), (POP) > (val_1));
})
DEF_CMD (EQ, 25, 0, push,
{
    stack_push (&(STACK), (POP) == (POP));
})
DEF_CMD (NEQ, 26, 0, push,
{
    stack_push (&(STACK), (POP) != (POP));
})
DEF_CMD (GEQ, 27, 0, push,
{
    val_1 = POP;

    stack_push (&(STACK), (POP) >= (val_1));
})
DEF_CMD (LEQ, 28, 0, push,
{
    val_1 = POP;

    stack_push (&(STACK), (POP) <= (val_1));
})
DEF_CMD (NEG, 29, 0, push,
{
    stack_push (&(STACK), (POP)*(-1));
})
