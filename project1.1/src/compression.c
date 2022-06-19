#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compression.h"
/* include headers */
int compress(const char *input_inst, RV32InstInfo *rv32inst, RV16InstInfo *rv16inst) {
    RV32InstInfo inst_info = newRV32InstInfo(input_inst); /* initialize */
    RV16InstInfo compressed_inst;
    int success_flag = 0; /* initialize success_flag and different soltion means different situation */
    *rv32inst = inst_info;
    switch(inst_info.inst_type) {/* use opcode to determine instruction type */
        case R: {
            if(inst_info.inst.R.funct3 == AddFunct3 && inst_info.inst.R.funct7 == AddFunct7 && inst_info.inst.R.opcode == 0x33) {
                /* use opcode, funct3, funct7 to determine instruction type */
                if(inst_info.inst.R.rs1 == inst_info.inst.R.rd && inst_info.inst.R.rd != 0 && inst_info.inst.R.rs2 != 0) {
                    /*  add rd, rd, rs2 (rd != x0, rs2 != x0)
                        -> C.AD rd rs
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CR; /* fill type */
                    compressed_inst.inst.CR.opcode = 0x2; /* fill compression type opcode */
                    compressed_inst.inst.CR.rs2 = inst_info.inst.R.rs2; /* fill compression type register */
                    compressed_inst.inst.CR.rs1 = inst_info.inst.R.rs1; /* fill compression type register */
                    compressed_inst.inst.CR.funct4 = 0x9; /* fill compression type funct */
                } else if(inst_info.inst.R.rs1 == 0 && inst_info.inst.R.rd != 0 && inst_info.inst.R.rs2 != 0 && inst_info.inst.R.opcode == 0x33) {
                    /*  add rd, x0, rs2 (rd != x0, rs2 != x0)
                        -> C.MV rd, rs2
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CR; /* fill type */
                    compressed_inst.inst.CR.opcode = 0x2; /* fill compression type opcode */
                    compressed_inst.inst.CR.rs2 = inst_info.inst.R.rs2; /* fill compression type register */
                    compressed_inst.inst.CR.rs1 = inst_info.inst.R.rd; /* fill compression type register */
                    compressed_inst.inst.CR.funct4 = 0x8; /* fill compression type funct */
                }
            } else if(inst_info.inst.R.funct3 == AndFunct3 && inst_info.inst.R.funct7 == AndFunct7 && inst_info.inst.R.opcode == 0x33) {
                /* use opcode, funct3, funct7 to determine instruction type */
                if(inst_info.inst.R.rs1 == inst_info.inst.R.rd 
                && inst_info.inst.R.rd >= 0x8 && inst_info.inst.R.rd <= 0xF 
                && inst_info.inst.R.rs2 >= 0x8 && inst_info.inst.R.rd <= 0xF) {
                    /*  and rd, rd, rs2 (0x8 <= rd <= 0xE, 0x8 <= rs2 <= 0xE)
                        -> C.AND rd - 0x8, rs2 - 0x8
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CS2; /* fill type */
                    compressed_inst.inst.CS2.opcode = 0x1; /* fill compression type opcode */
                    compressed_inst.inst.CS2.rs2 = inst_info.inst.R.rs2 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct2 = 0x3; /* fill compression type funct */
                    compressed_inst.inst.CS2.rs1 = inst_info.inst.R.rs1 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct6 = 0x23; /* fill compression type funct */
                }
            } else if(inst_info.inst.R.funct3 == OrFunct3 && inst_info.inst.R.funct7 == OrFunct7 && inst_info.inst.R.opcode == 0x33) {
                /* use opcode, funct3, funct7 to determine instruction type */
                if(inst_info.inst.R.rs1 == inst_info.inst.R.rd 
                && inst_info.inst.R.rd >= 0x8 && inst_info.inst.R.rd <= 0xF 
                && inst_info.inst.R.rs2 >= 0x8 && inst_info.inst.R.rd <= 0xF) {
                    /*  or rd, rd, rs2 (0x8 <= rd <= 0xE, 0x8 <= rs2 <= 0xE)
                        -> C.OR rd - 0x8, rs2 - 0x8
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CS2; /* fill type */
                    compressed_inst.inst.CS2.opcode = 0x1; /* fill compression type opcode */
                    compressed_inst.inst.CS2.rs2 = inst_info.inst.R.rs2 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct2 = 0x2; /* fill compression type funct */
                    compressed_inst.inst.CS2.rs1 = inst_info.inst.R.rs1 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct6 = 0x23; /* fill compression type funct */
                }
            } else if(inst_info.inst.R.funct3 == XorFunct3 && inst_info.inst.R.funct7 == XorFunct7 && inst_info.inst.R.opcode == 0x33) {
                /* use opcode, funct3, funct7 to determine instruction type */
                if(inst_info.inst.R.rs1 == inst_info.inst.R.rd 
                && inst_info.inst.R.rd >= 0x8 && inst_info.inst.R.rd <= 0xF 
                && inst_info.inst.R.rs2 >= 0x8 && inst_info.inst.R.rd <= 0xF) {
                    /*  xor rd, rd, rs2 (0x8 <= rd <= 0xE, 0x8 <= rs2 <= 0xE)
                        -> C.XOR rd - 0x8, rs2 - 0x8
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CS2; /* fill type */
                    compressed_inst.inst.CS2.opcode = 0x1; /* fill compression type opcode */
                    compressed_inst.inst.CS2.rs2 = inst_info.inst.R.rs2 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct2 = 0x1; /* fill compression type funct */
                    compressed_inst.inst.CS2.rs1 = inst_info.inst.R.rs1 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct6 = 0x23; /* fill compression type funct */
                }
            } else if(inst_info.inst.R.funct3 == SubFunct3 && inst_info.inst.R.funct7 == SubFunct7 && inst_info.inst.R.opcode == 0x33) {
                /* use opcode, funct3, funct7 to determine instruction type */
                if(inst_info.inst.R.rs1 == inst_info.inst.R.rd 
                && inst_info.inst.R.rd >= 0x8 && inst_info.inst.R.rd <= 0xF 
                && inst_info.inst.R.rs2 >= 0x8 && inst_info.inst.R.rd <= 0xF) {
                    /*  sub rd, rd, rs2 (0x8 <= rd <= 0xE, 0x8 <= rs2 <= 0xE)
                        -> C.SUB rd - 0x8, rs2 - 0x8
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CS2; /* fill type */
                    compressed_inst.inst.CS2.opcode = 0x1; /* fill compression type opcode */
                    compressed_inst.inst.CS2.rs2 = inst_info.inst.R.rs2 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct2 = 0x0; /* fill compression type funct */
                    compressed_inst.inst.CS2.rs1 = inst_info.inst.R.rs1 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS2.funct6 = 0x23; /* fill compression type funct */
                }
            }
            break;
        }
        case I: {
            /* calculate immediate */
            int imm = (inst_info.inst.I.imm & 0x7FF) - (inst_info.inst.I.imm & 0x800);
            if(inst_info.inst.I.funct3 == AddiFunct3 && inst_info.inst.I.opcode == 0x13) {
                /* use opcode, funct3 to determine instruction type */
                if(imm <= 31 && imm >= -32) {
                    /* check whether it can be compressed */
                    if(inst_info.inst.I.rd != 0 && inst_info.inst.I.rs1 == 0) {
                        /*  addi rd, x0, imm (rd != x0)
                            -> C.LI nzimm[5] rd imm[4:0] 
                        */
                        success_flag = 1;
                        compressed_inst.inst_type = CI; /* fill type */
                        compressed_inst.inst.CI.opcode = 0x01; /* fill compression type opcode */
                        compressed_inst.inst.CI.funct3 = 0x2; /* fill compression type funct */
                        compressed_inst.inst.CI.imm0_4 = inst_info.inst.I.imm & 0x1F; /* fill compression type imm */
                        compressed_inst.inst.CI.imm5 = (inst_info.inst.I.imm >> 11) & 0x01; /* fill compression type imm */
                        compressed_inst.inst.CI.rs1 = inst_info.inst.I.rd; /* fill compression type register */
                    } else if (inst_info.inst.I.rd == inst_info.inst.I.rs1 && inst_info.inst.I.imm != 0 && inst_info.inst.I.rd != 0) {
                        /*  addi rd, rd, nzimm (rd != x0)
                            -> C.ADDI nzimm[5] rd imm[4:0] 
                        */        
                        success_flag = 1;
                        compressed_inst.inst_type = CI; /* fill type */
                        compressed_inst.inst.CI.opcode = 0x01; /* fill compression type opcode */
                        compressed_inst.inst.CI.funct3 = 0x0; /* fill compression type funct */
                        compressed_inst.inst.CI.imm0_4 = inst_info.inst.I.imm & 0x1F; /* fill compression type imm */
                        compressed_inst.inst.CI.imm5 = (inst_info.inst.I.imm >> 11) & 0x01; /* fill compression type imm */
                        compressed_inst.inst.CI.rs1 = inst_info.inst.I.rs1; /* fill compression type register */
                    }
                }
            } else if(inst_info.inst.I.funct3 == JalrFunct3 && inst_info.inst.I.opcode == 0x67) {
                /* use opcode, funct3 to determine instruction type */
                if(inst_info.inst.I.rs1 != 0 && imm == 0) {
                    if(inst_info.inst.I.rd == 0) {
                        /*  jalr x0, 0 (rs1) (rs1 != x0)
                            -> C.JR rd
                        */
                        success_flag = 1;
                        compressed_inst.inst_type = CR; /* fill type */
                        compressed_inst.inst.CR.opcode = 0x2; /* fill compression type opcode */
                        compressed_inst.inst.CR.rs2 = 0; /* fill compression type register */
                        compressed_inst.inst.CR.rs1 = inst_info.inst.I.rs1; /* fill compression type register */
                        compressed_inst.inst.CR.funct4 = 0x8; /* fill compression type funct */
                    }
                    else if(inst_info.inst.I.rd == 1) {
                        /*  jalr x1, 0 (rs1) (rs1 != x0)
                            -> C.JALR rd
                        */
                        success_flag = 1;
                        compressed_inst.inst_type = CR; /* fill type */
                        compressed_inst.inst.CR.opcode = 0x2; /* fill compression type opcode */
                        compressed_inst.inst.CR.rs2 = 0; /* fill compression type register */
                        compressed_inst.inst.CR.rs1 = inst_info.inst.I.rs1; /* fill compression type register */
                        compressed_inst.inst.CR.funct4 = 0x9; /* fill compression type funct */
                    }
                }
            } else if (inst_info.inst.I.funct3 == SlliFunct3 && inst_info.inst.I.opcode == 0x13) {
                /* use opcode, funct3 to determine instruction type */
                /* if(imm <= 31 && imm >= 0) { */
                    /* check whether it can be compressed */
                    if(inst_info.inst.I.rs1 == inst_info.inst.I.rd && (inst_info.inst.I.imm >> 5) == 0x00) {
                        if(inst_info.inst.I.rd != 0) {
                            /*  slli rd, rd, shamt (rd != x0; shamt[5] must be zero)
                                -> C.SLLI imm[5] rd imm[4:0] 
                            */
                            success_flag = 1;
                            compressed_inst.inst_type = CI; /* fill type */
                            compressed_inst.inst.CI.opcode = 0x2; /* fill compression type opcode */
                            compressed_inst.inst.CI.funct3 = 0x0; /* fill compression type funct */
                            compressed_inst.inst.CI.rs1 = inst_info.inst.I.rs1; /* fill compression type register */
                            compressed_inst.inst.CI.imm5 = 0; /* fill compression type imm */
                            compressed_inst.inst.CI.imm0_4 = inst_info.inst.I.imm & 0x1F; /* fill compression type imm */
                        }
                    }
                /* } */
            } else if (inst_info.inst.I.funct3 == SrliFunct3 && inst_info.inst.I.opcode == 0x13) {
                /* use opcode, funct3 to determine instruction type */
                /* if(imm <= 31 && imm >= 0) { */
                    /* check whether it can be compressed */
                    if(inst_info.inst.I.rs1 == inst_info.inst.I.rd && (inst_info.inst.I.imm >> 5) == 0x00) {
                            if(inst_info.inst.I.rd >= 0x8 && inst_info.inst.I.rd <= 0xF) {
                            /*  srli rd', rd', shamt (shamt[5] must be zero)
                                -> C.SRLI imm[5] rd imm[4:0] 
                            */
                            success_flag = 1;
                            compressed_inst.inst_type = CB2; /* fill type */
                            compressed_inst.inst.CB2.opcode = 0x01; /* fill compression type opcode */
                            compressed_inst.inst.CB2.funct2 = 0x0; /* fill compression type funct */
                            compressed_inst.inst.CB2.funct3 = 0x4; /* fill compression type funct */
                            compressed_inst.inst.CB2.imm5 = 0; /* fill compression type imm */
                            compressed_inst.inst.CB2.imm0_4 = inst_info.inst.I.imm & 0x1F; /* fill compression type imm */
                            compressed_inst.inst.CB2.rs1 = inst_info.inst.I.rs1 - 0x8; /* fill compression type register */
                        }
                    }
                /* } */
            /* } else if (inst_info.inst.I.funct3 == SraiFunct3 && inst_info.inst.I.opcode == 0x13) { */
                /* use opcode, funct3 to determine instruction type */
                /* if(imm <= 31 && imm >= 0) { */
                    /* check whether it can be compressed */
                    if(inst_info.inst.I.rs1 == inst_info.inst.I.rd && (inst_info.inst.I.imm >> 5) == 0x20) {
                            if(inst_info.inst.I.rd >= 0x8 && inst_info.inst.I.rd <= 0xF) {
                            /*  srai rd', rd', shamt (shamt[5] must be zero)
                                -> C.SRAI nzimm[5] rd imm[4:0] 
                            */
                            success_flag = 1;
                            compressed_inst.inst_type = CB2; /* fill type */
                            compressed_inst.inst.CB2.opcode = 0x01; /* fill compression type opcode */
                            compressed_inst.inst.CB2.funct2 = 0x01; /* fill compression type funct */
                            compressed_inst.inst.CB2.funct3 = 0x4; /* fill compression type funct */
                            compressed_inst.inst.CB2.imm5 = 0x0; /* fill compression type imm */
                            compressed_inst.inst.CB2.imm0_4 = inst_info.inst.I.imm & 0x1F; /* fill compression type imm */
                            compressed_inst.inst.CB2.rs1 = inst_info.inst.I.rs1 - 0x8; /* fill compression type register */
                        } 
                    }
                /* } */
            } else if (inst_info.inst.I.funct3 == AndiFunct3 && inst_info.inst.I.opcode == 0x13) {
                /* use opcode, funct3 to determine instruction type */
                if(inst_info.inst.I.rs1 == inst_info.inst.I.rd) {
                    if(imm <= 31 && imm >= -32){
                        /* check whether it can be compressed */
                        if(inst_info.inst.I.rd >= 0x8 && inst_info.inst.I.rd <= 0xF) {
                            /*  andi rd', rd', imm
                                -> C.ANDI nzimm[5] rd imm[4:0] 
                            */
                            success_flag = 1;
                            compressed_inst.inst_type = CB2; /* fill type */
                            compressed_inst.inst.CB2.opcode = 0x01; /* fill compression type opcode */
                            compressed_inst.inst.CB2.funct2 = 0x02; /* fill compression type funct */
                            compressed_inst.inst.CB2.funct3 = 0x4; /* fill compression type funct */
                            compressed_inst.inst.CB2.imm5 = (inst_info.inst.I.imm >> 11) & 0x1; /* fill compression type imm */
                            compressed_inst.inst.CB2.imm0_4 = inst_info.inst.I.imm & 0x1F; /* fill compression type imm */
                            compressed_inst.inst.CB2.rs1 = inst_info.inst.I.rs1 - 0x8; /* fill compression type register */
                        } 
                    }
                }
            } else if (inst_info.inst.I.funct3 == LwFunct3 && inst_info.inst.I.opcode == 0x03) {
                /* use opcode, funct3 to determine instruction type */
                if(inst_info.inst.I.rd >= 0x8 && inst_info.inst.I.rd <= 0xF
                    && inst_info.inst.I.rs1 >= 0x8 && inst_info.inst.I.rs1 <= 0xF) {
                    if(imm <= 127 && imm >= 0 && imm % 4 == 0) {
                    /* if(imm % 4 == 0 && imm >= -4 * 16 && imm <= 15 * 4) { */
                        /* check whether it can be compressed */
                            /*  lw rd', offset (rs1')	
                                -> C.LW 
                            */
                        success_flag = 1;
                        compressed_inst.inst_type = CL; /* fill type */
                        compressed_inst.inst.CL.opcode = 0x00; /* fill compression type opcode */
                        compressed_inst.inst.CL.funct3 = 0x02; /* fill compression type funct */
                        compressed_inst.inst.CL.rd = inst_info.inst.I.rd - 0x8; /* fill compression type register */
                        compressed_inst.inst.CL.rs1 = inst_info.inst.I.rs1 - 0x8; /* fill compression type register */
                        compressed_inst.inst.CL.imm2 = (inst_info.inst.I.imm >> 2) & 0x1; /* fill compression type imm */
                        compressed_inst.inst.CL.imm6 = (inst_info.inst.I.imm >> 6) & 0x1; /* fill compression type imm */
                        compressed_inst.inst.CL.imm3_5 = (inst_info.inst.I.imm >> 3) & 0x7; /* fill compression type imm */
                    }
                }
            }
            break;
        }
        case S: {
            if(inst_info.inst.S.funct3 == SwFunct3 && inst_info.inst.S.opcode == 0x23) {
                /* use opcode, funct3 to determine instruction type */
                int imm0_4 = inst_info.inst.S.imm0_4, imm5_10 = (inst_info.inst.S.imm5_11 & 0x3F) << 5;
                int imm = imm0_4 + imm5_10 - (((inst_info.inst.S.imm5_11 >> 6) & 1) << 11);
                /* check whether it can be compressed */
                if(inst_info.inst.S.rs1 >= 0x8 && inst_info.inst.S.rs1 <= 0xF
                && inst_info.inst.S.rs2 >= 0x8 && inst_info.inst.S.rs2 <= 0xF
                && imm % 4 == 0 && imm >= 0 && imm <= 127) {
                /* && (inst_info.inst.S.imm0_4 & 0x3) == 0 && (inst_info.inst.S.imm5_11 >> 6) != 1
                && ((inst_info.inst.S.imm5_11 >> 2) == 0x0)) { */
                    /*  sw rs2, offset (rs1') 
                        -> C.SW offset[5:3] rs1' offset[2|6] rs2'
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CS1; /* fill type */
                    compressed_inst.inst.CS1.opcode = 0x00; /* fill compression type opcode */  
                    compressed_inst.inst.CS1.funct3 = 0x06; /* fill compression type funct */
                    compressed_inst.inst.CS1.rs1 = inst_info.inst.S.rs1 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS1.rs2 = inst_info.inst.S.rs2 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CS1.imm2 = (inst_info.inst.S.imm0_4 >> 2) & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CS1.imm6 = (inst_info.inst.S.imm5_11 >> 1) & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CS1.imm3_5 = ((inst_info.inst.S.imm5_11 & 0x1) << 2) + (inst_info.inst.S.imm0_4 >> 3); /* fill compression type imm */             
                }
            }
            break;
        }
        case B: {
            int imm = getRV32InstBImm(inst_info);
            if(!(imm >= -(1 << 8) && imm < (1 << 8) - 1 && (imm % 2 == 0))) break;
            if(inst_info.inst.B.funct3 == BeqFunct3 && inst_info.inst.B.opcode == 0x63) {
                /* use opcode, funct3 to determine instruction type */
                if(inst_info.inst.B.rs1 >= 0x8 && inst_info.inst.B.rs1 <= 0xF
                && inst_info.inst.B.rs2 == 0
                && ((inst_info.inst.B.imm5_10 >> 4) == 0x0  || (inst_info.inst.B.imm5_10 >> 4) == 0x3)) {
                    /*  beq rs1', x0, offset
                        -> C.BEQZ offset[8:6] rs1' offset[4:1|5]
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CB1; /* fill type */
                    compressed_inst.inst.CB1.opcode = 0x01; /* fill compression type opcode */
                    compressed_inst.inst.CB1.funct3 = 0x06; /* fill compression type funct */
                    compressed_inst.inst.CS1.rs1 = inst_info.inst.B.rs1 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CB1.imm1_2 = inst_info.inst.B.imm1_4 & 0x3; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm3_4 = inst_info.inst.B.imm1_4 >> 2; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm5 = inst_info.inst.B.imm5_10 & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm6_7 = (inst_info.inst.B.imm5_10 & 0x7) >> 1; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm8 = (inst_info.inst.B.imm5_10 >> 3) & 0x1; /* fill compression type imm */
                }
            } else if(inst_info.inst.B.funct3 == BneFunct3 && inst_info.inst.B.opcode == 0x63) {
                /* use opcode, funct3 to determine instruction type */
                if(inst_info.inst.B.rs1 >= 0x8 && inst_info.inst.B.rs1 <= 0xF
                && inst_info.inst.B.rs2 == 0
                && ((inst_info.inst.B.imm5_10 >> 4) == 0x0  || (inst_info.inst.B.imm5_10 >> 4) == 0x3)) {
                    /*  bne rs1', x0, offset
                        -> C.BNEZ offset[8:6] rs1' offset[4:1|5]
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CB1; /* fill type */
                    compressed_inst.inst.CB1.opcode = 0x01; /* fill compression type opcode */
                    compressed_inst.inst.CB1.funct3 = 0x07; /* fill compression type funct */
                    compressed_inst.inst.CS1.rs1 = inst_info.inst.B.rs1 - 0x8; /* fill compression type register */
                    compressed_inst.inst.CB1.imm1_2 = inst_info.inst.B.imm1_4 & 0x3; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm3_4 = inst_info.inst.B.imm1_4 >> 2; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm5 = inst_info.inst.B.imm5_10 & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm6_7 = (inst_info.inst.B.imm5_10 & 0x7) >> 1; /* fill compression type imm */
                    compressed_inst.inst.CB1.imm8 = (inst_info.inst.B.imm5_10 & 0xF) >> 3; /* fill compression type imm */
                }
            }
            break;
        }
        case U: {
            int imm = (inst_info.inst.U.imm12_31 & 0x7FFFF) - (inst_info.inst.U.imm12_31 & 0x80000);
            /* use opcode to determine instruction type */
            if(inst_info.inst.U.rd != 0 && inst_info.inst.U.rd != 2 && inst_info.inst.U.imm12_31 != 0 
            && inst_info.inst.U.opcode == 0x37 
            && imm >= -(1 << 5) && imm < (1 << 5)
            /*((inst_info.inst.U.imm12_31 >> 6) == 0x0 || (inst_info.inst.U.imm12_31 >> 6) == 0x3FFF)*/) {
                /*  lui rd, nzimm (rd != x0, rd != x2, nzimm != x0)
                    -> C.LUI rd nzimm[17:12]
                */
                success_flag = 1;
                compressed_inst.inst_type = CI; /* fill type */
                compressed_inst.inst.CI.opcode = 0x01; /* fill compression type opcode */
                compressed_inst.inst.CI.funct3 = 0x03; /* fill compression type funct */
                compressed_inst.inst.CI.rs1 = inst_info.inst.U.rd; /* fill compression type register */
                compressed_inst.inst.CI.imm0_4 = inst_info.inst.U.imm12_31 & 0x1F; /* fill compression type imm */
                compressed_inst.inst.CI.imm5 = (inst_info.inst.U.imm12_31 >> 19) & 0x01; /* fill compression type imm */
            }
            break;
        }
        case J: {
            int imm = getRV32InstJImm(inst_info);
            if(!(imm >= -(1 << 11) && imm < (1 << 11) - 1 && (imm % 2 == 0))) break;
            if(inst_info.inst.J.rd == 0 && inst_info.inst.J.opcode == 0x6F) {
                    /*  jal x0, offset
                        -> C.J offset[11:6|4:1|5]
                    */
                    success_flag = 1;
                    compressed_inst.inst_type = CJ; /* fill type */
                    compressed_inst.inst.CJ.opcode = 0x01; /* fill compression type opcode */
                    compressed_inst.inst.CJ.funct3 = 0x05; /* fill compression type funct */
                    compressed_inst.inst.CJ.imm1_3 = inst_info.inst.J.imm1_10 & 0x7; /* fill compression type imm */
                    compressed_inst.inst.CJ.imm4 = (inst_info.inst.J.imm1_10 >> 3) & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CJ.imm5 = (inst_info.inst.J.imm1_10 >> 4) & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CJ.imm6 = (inst_info.inst.J.imm1_10 >> 5) & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CJ.imm7 = (inst_info.inst.J.imm1_10 >> 6) & 0x1; /* fill compression type imm */
                    compressed_inst.inst.CJ.imm8_9 = (inst_info.inst.J.imm1_10 >> 7) & 0x3; /* fill compression type imm */
                    compressed_inst.inst.CJ.imm10 = (inst_info.inst.J.imm1_10 >> 9); /* fill compression type imm */
                    compressed_inst.inst.CJ.imm11 = inst_info.inst.J.imm11; /* fill compression type imm */
            } else if (inst_info.inst.J.rd == 1 && inst_info.inst.J.opcode == 0x6F) {
                /*  jal x1, offset
                    -> C.J offset[11:6|4:1|5]
                */
                success_flag = 1;
                compressed_inst.inst_type = CJ; /* fill type */
                compressed_inst.inst.CJ.opcode = 0x01; /* fill compression type opcode */
                compressed_inst.inst.CJ.funct3 = 0x01; /* fill compression type funct */
                compressed_inst.inst.CJ.imm1_3 = inst_info.inst.J.imm1_10 & 0x7; /* fill compression type imm */
                compressed_inst.inst.CJ.imm4 = (inst_info.inst.J.imm1_10 >> 3) & 0x1; /* fill compression type imm */
                compressed_inst.inst.CJ.imm5 = (inst_info.inst.J.imm1_10 >> 4) & 0x1; /* fill compression type imm */
                compressed_inst.inst.CJ.imm6 = (inst_info.inst.J.imm1_10 >> 5) & 0x1; /* fill compression type imm */
                compressed_inst.inst.CJ.imm7 = (inst_info.inst.J.imm1_10 >> 6) & 0x1; /* fill compression type imm */
                compressed_inst.inst.CJ.imm8_9 = (inst_info.inst.J.imm1_10 >> 7) & 0x3; /* fill compression type imm */
                compressed_inst.inst.CJ.imm10 = (inst_info.inst.J.imm1_10 >> 9); /* fill compression type imm */
                compressed_inst.inst.CJ.imm11 = inst_info.inst.J.imm11; /* fill compression type imm */
            }
            break;
        }
        case ERROR32: {
            return -1; /* unexpected error code */
            break;
        }

    }
    if(success_flag) {
        /*  while(index < 16) {
                output_inst[index] = (compressed_inst.inst.code >> index & 1) + '0';
                index++; 
            }
            output_inst[index] = 0;
        */
        *rv16inst = compressed_inst; /* if we success compress the instruction, write to rv16inst. */
    } /* else {
        strcpy(output_inst, input_inst);
        reverse(output_inst);
    } */
    return success_flag;
}