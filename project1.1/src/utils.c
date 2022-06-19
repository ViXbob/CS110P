#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/*  swap two char variables
*/
void swap(char *a, char *b) {
    /* use a temporary variable to achieve it */
    char tmp = *b;
    *b = *a;
    *a = tmp;
}

/*  reverse a string
*/
void reverse(char *str) {
    uint32 len = strlen(str), l, r;
    if(len <= 0) return; /* can not swap a string whose length is no more than 0 */
    l = 0; r = len - 1;
    /* use two pointers, one is forwarding and the other is backwarding */
    while(l < r) {
        swap(str + l, str + r);
        l++; r--;
    }
}

/*  translate binary inst[l : r-1] into decimal
*/
RV32Inst getCode(const char *inst, int l, int r) {
    int i = l;
    RV32Inst code = 0;
    /* from most significant bit to least significant bit */
    while(i < r) {
        code = code * 2 + inst[i] - '0';
        i++;
    }
    return code; /* return code */
}

/*  get type of 32bits-instruction
*/
RV32InstType getRVInst32Type(RV32InstInfo inst_info) {
    RV32Inst opcode = inst_info.inst.R.opcode;
    /* determine the type by opcode */
    if(opcode == 0x33) return R;      /* opcode = 0x33 */
    else if(opcode == 0x13) return I; /* opcode = 0x13 */
    else if(opcode == 0x03) return I; /* opcode = 0x03 */
    else if(opcode == 0x23) return S; /* opcode = 0x23 */
    else if(opcode == 0x63) return B; /* opcode = 0x63 */
    else if(opcode == 0x6F) return J; /* opcode = 0x6F */
    else if(opcode == 0x67) return I; /* opcode = 0x67 */
    else if(opcode == 0x37) return U; /* opcode = 0x37 */
    else if(opcode == 0x17) return U; /* opcode = 0x17 */
    else if(opcode == 0x73) return I; /* opcode = 0x73 */
    else return ERROR32;
}

/*  get RV32InstInfo from a binary string
*/
RV32InstInfo newRV32InstInfo(const char *inst) {
    RV32Inst code = getCode(inst, 0, 32);
    RV32InstInfo inst_info;
    /* in union struct, we can just assign the value of code */
    inst_info.inst.code = code;
    /* set the type of instruction */
    inst_info.inst_type = getRVInst32Type(inst_info);
    return inst_info;
}

/* convert a RV32InstInfo into a 32-bits bainary string
*/
void RV32InstInfo2String(const RV32InstInfo inst_info, char *output_inst) {
    int index = 0;
    /* get each char from bit to bit */
    while(index < 32) {
        output_inst[index] = (inst_info.inst.code >> index & 1) + '0';
        index++;
    }
    /* add null char */
    output_inst[index] = 0;
    /* reverse to print string */
    reverse(output_inst);
}

/* convert a RV16InstInfo into a 16-bits bainary string
*/
void RV16InstInfo2String(const RV16InstInfo inst_info, char *output_inst) {
    int index = 0;
    /* get each char from bit to bit */
    while(index < 16) {
        output_inst[index] = (inst_info.inst.code >> index & 1) + '0';
        index++;
    }
    /* add null char */
    output_inst[index] = 0;
    /* reverse to print string */
    reverse(output_inst);
}

/* get immediate field from Type B RV32 instruction
*/
int getRV32InstBImm(RV32InstInfo inst_info) {
    int imm11, imm12, imm1_4, imm5_10;
    if(inst_info.inst_type != B) {
        /* Can not get immediate from a instruction whose type is not B! */
        printf("ERROR: Can not get immediate from a instruction whose type is not B!\n");
        exit(0);
    }
    /* get each part of immediate */
    imm11 = inst_info.inst.B.imm11 << 11;
    imm12 = inst_info.inst.B.imm12 << 12;
    imm1_4 = inst_info.inst.B.imm1_4 << 1;
    imm5_10 = inst_info.inst.B.imm5_10 << 5;
    /* return value */
    return imm1_4 + imm5_10 + imm11 - imm12;
}

/* get immediate field from Type CB1 RV16 instruction
*/
int getRV16InstCB1Imm(RV16InstInfo inst_info) {
    int imm1_2, imm3_4, imm5, imm6_7, imm8;
    if(inst_info.inst_type != CB1) {
        /* Can not get immediate from a instruction whose type is not CB1! */
        printf("ERROR: Can not get immediate from a instruction whose type is not CB1!\n");
        exit(0);
    }
    /* get each part of immediate */
    imm1_2 = inst_info.inst.CB1.imm1_2 << 1;
    imm3_4 = inst_info.inst.CB1.imm3_4 << 3;
    imm5 = inst_info.inst.CB1.imm5 << 5;
    imm6_7 = inst_info.inst.CB1.imm6_7 << 6;
    /* signed bit */
    imm8 = inst_info.inst.CB1.imm8 << 8;
    /* return value */
    return imm1_2 + imm3_4 + imm5 + imm6_7 - imm8;
}

/* set immediate field from Type B RV32 instruction
*/
void setRV32InstBImm(RV32InstInfo *inst_info, int imm) {
    if(inst_info -> inst_type != B) {
        /* Can not set immediate to a instruction whose type is not B! */
        printf("ERROR: Can not set immediate to a instruction whose type is not B!\n");
        exit(0);
    }
    if(!(imm >= -(1 << 12) && imm < (1 << 12) - 1 && (imm % 2 == 0))) {
        /* The offset to be set to type B is outbounded! */
        printf("ERROR: The offset to be set to type B is outbounded!\n");
        exit(0);
    }
    /* set each part of immediate */
    inst_info -> inst.B.imm11 = (imm >> 11) & 0x1;
    inst_info -> inst.B.imm12 = (imm >> 12) & 0x1;
    inst_info -> inst.B.imm1_4 = (imm >> 1) & 0xF;
    inst_info -> inst.B.imm5_10 = (imm >> 5) & 0x3F;
}

/* set immediate field from Type CB1 RV16 instruction
*/
void setRV16InstCB1Imm(RV16InstInfo *inst_info, int imm) {
    if(inst_info -> inst_type != CB1) {
        /* Can not set immediate to a instruction whose type is not CB! */
        printf("ERROR: Can not set immediate to a instruction whose type is not CB1!\n");
        exit(0);
    }
    if(!(imm >= -(1 << 8) && imm < (1 << 8) - 1 && (imm % 2 == 0))) {
        /* The offset to be set to type CB1 is outbounded! */
        printf("ERROR: The offset to be set to type CB1 is outbounded!\n");
        exit(0);
    }
    /* set each part of immediate */
    inst_info -> inst.CB1.imm1_2 = (imm >> 1) & 0x3;
    inst_info -> inst.CB1.imm3_4 = (imm >> 3) & 0x3;
    inst_info -> inst.CB1.imm5 = (imm >> 5) & 0x1;
    inst_info -> inst.CB1.imm6_7 = (imm >> 6) & 0x3;
    /* signed bit */
    inst_info -> inst.CB1.imm8 = (imm >> 8) & 0x1;
}

/* get immediate field from Type J RV32 instruction
*/
int getRV32InstJImm(RV32InstInfo inst_info) {
    int imm12_19, imm11, imm1_10, imm20;
    if(inst_info.inst_type != J) {
        /* Can not get immediate from a instruction whose type is not J! */
        printf("ERROR: Can not get immediate from a instruction whose type is not J!\n");
        exit(0);
    }
    /* get each part of immediate */
    imm12_19 = inst_info.inst.J.imm12_19 << 12;
    imm11 = inst_info.inst.J.imm11 << 11;
    imm1_10 = inst_info.inst.J.imm1_10 << 1;
    imm20 = inst_info.inst.J.imm20 << 20;
    /* return value */
    return imm1_10 + imm11 + imm12_19 - imm20;
}

/* get immediate field from Type CJ RV16 instruction
*/
int getRV16InstCJImm(RV16InstInfo inst_info) {
    int imm1_3, imm4, imm5, imm6, imm7, imm8_9, imm10, imm11;
    if(inst_info.inst_type != CJ) {
        /* Can not get immediate from a instruction whose type is not CJ! */
        printf("ERROR: Can not get immediate from a instruction whose type is not CJ!\n");
        exit(0);
    }
    /* get each part of immediate */
    imm1_3 = inst_info.inst.CJ.imm1_3 << 1;
    imm4 = inst_info.inst.CJ.imm4 << 4;
    imm5 = inst_info.inst.CJ.imm5 << 5;
    imm6 = inst_info.inst.CJ.imm6 << 6;
    /* get other four parts of immediate */
    imm7 = inst_info.inst.CJ.imm7 << 7;
    imm8_9 = inst_info.inst.CJ.imm8_9 << 8;
    imm10 = inst_info.inst.CJ.imm10 << 10;
    imm11 = inst_info.inst.CJ.imm11 << 11;
    /* return value */
    return imm1_3 + imm4 + imm5 + imm6 + imm7 + imm8_9 + imm10 - imm11;
}

/* set immediate field from Type J RV32 instruction
*/
void setRV32InstJImm(RV32InstInfo *inst_info, int imm) {
    if(inst_info -> inst_type != J) {
        /* Can not set immediate to a instruction whose type is not J! */
        printf("ERROR: Can not set immediate to a instruction whose type is not J!\n");
        exit(0);
    }
    if(!(imm >= -(1 << 20) && imm < (1 << 20) - 1 && (imm % 2 == 0))) {
        /* The offset to be set to type J is outbounded! */
        printf("ERROR: The offset to be set to type J is outbounded!\n");
        exit(0);
    }
    /* set each part of immediate */
    inst_info -> inst.J.imm1_10 = (imm >> 1) & 0x3FF;
    inst_info -> inst.J.imm11 = (imm >> 11) & 0x1;
    inst_info -> inst.J.imm12_19 = (imm >> 12) & 0xFF;
    inst_info -> inst.J.imm20 = (imm >> 20) & 0x1;
}

/* set immediate field from Type CJ RV16 instruction
*/
void setRV16InstCJImm(RV16InstInfo *inst_info, int imm) {
    if(inst_info -> inst_type != CJ) {
        /* Can not set immediate to a instruction whose type is not CJ! */
        printf("ERROR: Can not set immediate to a instruction whose type is not CJ!\n");
        exit(0);
    }
    if(!(imm >= -(1 << 11) && imm < (1 << 11) - 1 && (imm % 2 == 0))) {
        /* ERROR: The offset to be set to type CJ is outbounded! */
        printf("ERROR: The offset to be set to type CJ is outbounded!\n");
        exit(0);
    }
    /* set each part of immediate */
    inst_info -> inst.CJ.imm1_3 = (imm >> 1) & 0x7;
    inst_info -> inst.CJ.imm4 = (imm >> 4) & 0x1;
    inst_info -> inst.CJ.imm5 = (imm >> 5) & 0x1;
    inst_info -> inst.CJ.imm6 = (imm >> 6) & 0x1;
    /* set other four parts of immediate */
    inst_info -> inst.CJ.imm7 = (imm >> 7) & 0x1;
    inst_info -> inst.CJ.imm8_9 = (imm >> 8) & 0x3;
    inst_info -> inst.CJ.imm10 = (imm >> 10) & 0x1;
    /* set signed bit */
    inst_info -> inst.CJ.imm11 = (imm >> 11) & 0x1;
}