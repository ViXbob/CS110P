#ifndef UTILS_H
#define UTILS_H
#define AddFunct3 0x0
#define AddFunct7 0x00
#define AndFunct3 0x7
#define AndFunct7 0x00
#define OrFunct3  0x6
#define OrFunct7  0x00
#define XorFunct3 0x4
#define XorFunct7 0x00
#define SubFunct3 0x0
#define SubFunct7 0x20
#define AddiFunct3 0x0
#define JalrFunct3 0x0
#define SlliFunct3 0x1
#define SrliFunct3 0x5
#define SraiFunct3 0x5
#define AndiFunct3 0x7
#define LwFunct3 0x2
#define SwFunct3 0x2
#define BeqFunct3 0x0
#define BneFunct3 0x1

/* enumeration of RV32 instruction type */
typedef enum _InstType32 {R, I, S, B, U, J, ERROR32} RV32InstType;
/* enumeration of RV16 instruction type */
typedef enum _InstType16 {CR, CI, CL, CS1, CS2, CB1, CB2, CJ, ERROR16} RV16InstType;
/* typedef of unsigned 32bits-int */
typedef __uint32_t RV32Inst;
typedef __uint32_t uint32;
typedef __uint32_t RV16Inst;

typedef struct _RV32InstInfo {
    /* use bitfeild and union to define of RV32 instruction information */
    union {
        /* definition of RV32 instruction of type R */
        struct {
            RV32Inst opcode   : 7; /* opcode occupies 7 bits */
            RV32Inst rd       : 5; /* rd occupies 5 bits */
            RV32Inst funct3   : 3;
            RV32Inst rs1      : 5; /* rs1 occupies 5 bits */
            RV32Inst rs2      : 5;
            RV32Inst funct7   : 7; /* funct7 occupies 7 bits */
        } R;

        /* definition of RV32 instruction of type I */
        struct {
            RV32Inst opcode   : 7; /* opcode occupies 7 bits */
            RV32Inst rd       : 5; /* rd occupies 5 bits */
            RV32Inst funct3   : 3;
            RV32Inst rs1      : 5; /* rs1 occupies 5 bits */
            RV32Inst imm      : 12;
        } I;

        /* definition of RV32 instruction of type S */
        struct {
            RV32Inst opcode   : 7; /* opcode occupies 7 bits */
            RV32Inst imm0_4   : 5;
            RV32Inst funct3   : 3;
            RV32Inst rs1      : 5; /* rs1 occupies 5 bits */
            RV32Inst rs2      : 5;
            RV32Inst imm5_11  : 7; /* immediate[5:11] occupies 7 bits */
        } S;

        /* definition of RV32 instruction of type B */
        struct {
            RV32Inst opcode   : 7; /* opcode occupies 7 bits */
            RV32Inst imm11    : 1;
            RV32Inst imm1_4   : 4;
            RV32Inst funct3   : 3;
            RV32Inst rs1      : 5; /* rs1 occupies 5 bits */
            RV32Inst rs2      : 5;
            RV32Inst imm5_10  : 6; /* immediate[5:10] occupies 6 bits */
            RV32Inst imm12    : 1;
        } B;

        /* definition of RV32 instruction of type U */
        struct {
            RV32Inst opcode   : 7;  /* opcode occupies 7 bits */
            RV32Inst rd       : 5;  /* rd occupies 5 bits */
            RV32Inst imm12_31 : 20; /* immediate[12:31] occupies 20 bits */
        } U;

        /* definition of RV32 instruction of type J */
        struct {
            RV32Inst opcode   : 7;  /* opcode occupies 7 bits */
            RV32Inst rd       : 5;  /* rd occupies 5 bits */
            RV32Inst imm12_19 : 8;  /* immediate[12:19] occupies 8 bits */
            RV32Inst imm11    : 1;
            RV32Inst imm1_10  : 10; /* immediate[1:10] occupies 10 bits */
            RV32Inst imm20    : 1;  /* immediate[20:20] occupies 1 bits */
        } J;
        RV32Inst code;
    } inst;
    RV32InstType inst_type; /* definition of RV32 instruction type */
} RV32InstInfo;

typedef struct _RV16InstInfo {
    /* use bitfeild and union to define of RV16 instruction information */
    union {
        /* definition of RV16 instruction of type CR */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst rs2      : 5;
            RV16Inst rs1      : 5; /* rs1 occupies 5 bits */
            RV16Inst funct4   : 4; /* funct4 occupies 4 bits */
        } CR;

        /* definition of RV16 instruction of type CI */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst imm0_4   : 5;
            RV16Inst rs1      : 5; /* rs1 occupies 5 bits */
            RV16Inst imm5     : 1;
            RV16Inst funct3   : 3; /* funct3 occupies 3 bits */
        } CI;

        /* definition of RV16 instruction of type CL */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst rd       : 3;
            RV16Inst imm6     : 1; /* immediate[6:6] occupies 1 bits */
            RV16Inst imm2     : 1;
            RV16Inst rs1      : 3; /* rs1 occupies 3 bits */
            RV16Inst imm3_5   : 3;
            RV16Inst funct3   : 3; /* funct3 occupies 3 bits */
        } CL;

        /* definition of RV16 instruction of type CS1 */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst rs2      : 3;
            RV16Inst imm6     : 1; /* immediate[6:6] occupies 1 bits */
            RV16Inst imm2     : 1;
            RV16Inst rs1      : 3; /* rs1 occupies 3 bits */
            RV16Inst imm3_5   : 3;
            RV16Inst funct3   : 3; /* funct3 occupies 3 bits */
        } CS1;

        /* definition of RV16 instruction of type CS2 */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst rs2      : 3;
            RV16Inst funct2   : 2;
            RV16Inst rs1      : 3; /* rs1 occupies 3 bits */
            RV16Inst funct6   : 6; /* funct6 occupies 6 bits */
        } CS2;

        /* definition of RV16 instruction of type CB1 */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst imm5     : 1;
            RV16Inst imm1_2   : 2;
            RV16Inst imm6_7   : 2; /* immediate[6:7] occupies 2 bits */
            RV16Inst rs1      : 3; /* rs1 occupies 3 bits */
            RV16Inst imm3_4   : 2;
            RV16Inst imm8     : 1;
            RV16Inst funct3   : 3; /* funct3 occupies 3 bits */
        } CB1;

        /* definition of RV16 instruction of type CB2 */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst imm0_4   : 5;
            RV16Inst rs1      : 3; /* rs1 occupies 3 bits */
            RV16Inst funct2   : 2;
            RV16Inst imm5     : 1;
            RV16Inst funct3   : 3; /* funct3 occupies 3 bits */
        } CB2;

        /* definition of RV16 instruction of type CJ */
        struct {
            RV16Inst opcode   : 2; /* opcode occupies 2 bits */
            RV16Inst imm5     : 1;
            RV16Inst imm1_3   : 3; /* immediate[1:3] occupies 3 bits */
            RV16Inst imm7     : 1;
            RV16Inst imm6     : 1; /* immediate[6:6] occupies 1 bits */
            RV16Inst imm10    : 1;
            RV16Inst imm8_9   : 2; /* immediate[8:9] occupies 2 bits */
            RV16Inst imm4     : 1;
            RV16Inst imm11    : 1;
            RV16Inst funct3   : 3; /* funct3 occupies 3 bits */
        } CJ;
        RV16Inst code;
    } inst;
    RV16InstType inst_type; /* definition of RV16 instruction type */
} RV16InstInfo;

/*  swap two char variables
*/
void swap(char *a, char *b);

/*  reverse a string
*/
void reverse(char *str);

/*  translate binary inst[l : r-1] into decimal
*/
RV32Inst getCode(const char *inst, int l, int r);

/*  get type of 32bits-instruction
*/
RV32InstType getRVInst32Type(RV32InstInfo inst_info);

/*  get RV32InstInfo from a binary string
*/
RV32InstInfo newRV32InstInfo(const char *inst);

/* convert a RV32InstInfo into a 32-bits bainary string
*/
void RV32InstInfo2String(RV32InstInfo inst_info, char *output_inst);

/* convert a RV16InstInfo into a 16-bits bainary string
*/
void RV16InstInfo2String(RV16InstInfo inst_info, char *output_inst);

/* get immediate field from Type B RV32 instruction
*/
int getRV32InstBImm(RV32InstInfo inst_info);

/* get immediate field from Type CB1 RV16 instruction
*/
int getRV16InstCB1Imm(RV16InstInfo inst_info);

/* set immediate field from Type B RV32 instruction
*/
void setRV32InstBImm(RV32InstInfo *inst_info, int imm);

/* set immediate field from Type CB1 RV16 instruction
*/
void setRV16InstCB1Imm(RV16InstInfo *inst_info, int imm);

/* get immediate field from Type J RV32 instruction
*/
int getRV32InstJImm(RV32InstInfo inst_info);

/* get immediate field from Type CJ RV16 instruction
*/
int getRV16InstCJImm(RV16InstInfo inst_info);

/* set immediate field from Type J RV32 instruction
*/
void setRV32InstJImm(RV32InstInfo *inst_info, int imm);

/* set immediate field from Type CJ RV16 instruction
*/
void setRV16InstCJImm(RV16InstInfo *inst_info, int imm);

#endif