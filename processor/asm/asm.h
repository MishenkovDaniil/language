#ifndef ASM_H
#define ASM_H

#include "../labels.h"

static const int LABEL_NAME = 0;
static const int ERROR = 0;


int is_label_name (Label *label, const char *jmp_name);
int init_code (char *text, int *op_code, Label *label);
void write_opcode (FILE *out_file, int *op_code, int n_instr, const int magic_number, const int version);
void assemble_pop_arg (char **text, int *op_code, int *ip, Label *label, int *label_index, int *err);
int assemble_push_arg (char **text, int *op_code, int *ip, Label *label, int *label_index, int *err);
void assemble_jmp_arg (char **text, int *op_code, int *ip, Label *label, int *label_index, int *err);
int assemble_label_arg (char *cmd, int *ip, Label *label, int *label_index, int *err);

#endif /* ASM_H */
