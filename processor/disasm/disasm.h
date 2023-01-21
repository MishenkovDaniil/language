#ifndef DISASM_H
#define DISASM_H

void diasm_code (const char *disasm, Label *lalbels, Header header, int *instrs);
void process_push_arg (FILE *disasm_file, Label *labels, int cmd, int *instrs, int *index, int *label_index);
void process_jmp_arg (FILE *disasm_file, Label *labels, int cmd, int *instrs, int *index, int *label_index);
int find_label (Label *labels, int value);


#endif /* DISASM_H */
