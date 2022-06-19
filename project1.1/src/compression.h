#ifndef COMPRESSION_H
#define COMPRESSION_H
#include "utils.h"

/*  Take an input instruction and put the result in output_inst
    Return value:
        0: failed to compress
        1: compress 32-bits instruction into 16-bits successfully
       -1: unexpected error
*/
int compress(const char *input_inst, RV32InstInfo *rv32inst, RV16InstInfo *rv16inst);

#endif