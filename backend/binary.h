#ifndef BINARY_H
#define BINARY_H


const int MAGIC_NUMBER = 0x00005A4D; //
const int VERSION = 1;          //
const int MAX_OP_LEN = 2;       //

struct Header
{
    int magic_number = 0;
    int file_version = 0;
    int n_instr      = 0;
};

#endif /* BINARY_H */
