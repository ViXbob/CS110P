/*  Project 1.1: RISC-V instructions to RISC-V compressed instructions in C89.
    The following is the starter code provided for you. To finish the task, you 
    should define and implement your own functions in translator.c, compression.c, 
    utils.c and their header files.
    Please read the problem description before you start.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "src/compression.h"
#include "src/utils.h"

#include "translator.h"

/*check if file can be correctly opened */
static int open_files(FILE** input, FILE** output, const char* input_name, const char* output_name){ 
    *input = fopen(input_name, "r");
    if (!*input){ /* open input file failed */
        printf("Error: unable to open input file: %s\n", input_name);
        return -1;
    }

    *output = fopen(output_name, "w");
    if (!*output){ /* open output file failed */
        printf("Error: unable to open output file: %s\n", output_name);
        fclose(*input);
        return -1;
    }
    return 0; /* no problem opening files */
}

static int close_files(FILE** input, FILE** output){
    fclose(*input);
    fclose(*output); /* close the files at the end */
    return 0;
}

static void print_usage_and_exit() {
    printf("Usage:\n");
    printf("Run program with translator <input file> <output file>\n"); /* print the correct usage of the program */
    exit(0);
}

/* Compress the instruction in file "in" and put the result in file "out" */
int translate(const char*in, const char*out){
    FILE *input, *output;
    /* definition of variable */
    int err = 0, result_code, cnt = 0, index = 0;
    char input_inst[33], output_inst[33];
    RV16InstInfo rv16inst[1005];
    /* definition of other variables */
    RV32InstInfo rv32inst[1005];
    int compressed_flag[1005], presum[1005], offset = 0, offset_index = 0;
    if (in){    /* correct input file name */
        if(open_files(&input, &output, in, out) != 0)
            exit(1);
        /* ... */
        /* write correct result to the output file */
        /* read instruction from input file */
        while(~fscanf(input, "%s", input_inst)) {
            /* compress instruction and get result code */
            result_code = compress(input_inst, rv32inst + cnt, rv16inst + cnt);
            /* Unexpected error, return */
            if(result_code == -1) {
                err = -1;
                break;
            }
            /* store result code */
            compressed_flag[cnt] = result_code;
            cnt++;
            /*  reverse(output_inst);
                fprintf(output, "%s\n", output_inst);
            */
        }
        /* get presum of instruction size */
        presum[0] = 0;
        for(index = 1; index < cnt; index++) {
            presum[index] = presum[index - 1] + (compressed_flag[index - 1] == 0 ? 4 : 2);
        }
        /* Correct the instruction which has offset field after compression */
        for(index = 0; index < cnt; index++) {
            /* Case of type B instruction */
            if(rv32inst[index].inst_type == B) {
                /* get the correct offset */
                offset = getRV32InstBImm(rv32inst[index]);
                offset_index = offset / 4;
                offset = presum[index + offset_index] - presum[index];
                /* set the correct offset */
                if(compressed_flag[index] == 0) {
                    /* RV32 instruction */
                    setRV32InstBImm(rv32inst + index, offset);
                } else {
                    /* RV16 instruction */
                    setRV16InstCB1Imm(rv16inst + index, offset);
                }
            } else if(rv32inst[index].inst_type == J) {
                /* Case of type J instruction */
                /* get the correct offset */
                offset = getRV32InstJImm(rv32inst[index]);
                offset_index = offset / 4;
                offset = presum[index + offset_index] - presum[index];
                /* set the correct offset */
                if(compressed_flag[index] == 0) {
                    /* RV32 instruction */
                    setRV32InstJImm(rv32inst + index, offset);
                } else {
                    /* RV16 instruction */
                    setRV16InstCJImm(rv16inst + index, offset);
                }
            }
            if(compressed_flag[index] == 0) {
                /* convert un-compressed instruction to string */
                RV32InstInfo2String(rv32inst[index], output_inst);
            } else {
                /* convert compressed instruction to string */
                RV16InstInfo2String(rv16inst[index], output_inst);
            }
            /* write result to file */
            fprintf(output, "%s\n", output_inst);
        }
        /* ... */
        close_files(&input, &output);
    }
    /* return error code */
    return err;
}

/* main func */
int main(int argc, char **argv){
    char* input_fname, *output_fname;
    int err;

    if (argc != 3) /* need correct arguments */
        print_usage_and_exit();

    input_fname = argv[1];
    output_fname = argv[2];

    err = translate(input_fname, output_fname); /* main translation process */
    if (err)
        printf("One or more errors encountered during translation operation.\n"); /* something wrong */
    else
        printf("Translation process completed successfully.\n"); /* correctly output */

    return 0;
}