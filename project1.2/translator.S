#   Project 1.2 RVC instructions to RISC-V instructions in RISC-V

#   You can suppose you will get the number of machine codes you should translate from 
#   "lines_of_rvc_codes" and get the 16 or 32 bits-machine codes from "rvc_codes". 
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input_full_1.S
# .import input_full.S
# .import input_single.S
# Your code to convert the RVC codes to RVI codes.
# main function uses s1 - s8
main:
    lw s4, lines_of_rvc_codes
    la s5, rvc_codes
    # mollac an array to record the output instruction
    # s2 represents the pointer of output insturctions
    slli, t2, s4, 2
    sub sp, sp, t2
    addi, s2, sp, 0
    # mollac an array to reocrd the length of input instructions
    # s3 represents the pointer of length of input instructions
    sub sp, sp, t2
    addi, s3, sp, 0
    # initialize loop variable s1, s6
    li s1, 0
    addi s6, s5, 0
    load_instr:
        # break condition
        slli t2, s4, 2
        bge s1, t2, break_load_instr

        # load byte to check opcode
        lhu t2, 0(s6)

        andi t3, t2, 0b11
        addi t3, t3, -0b11
        beq t3, x0, donot_need_to_extension
        bne t3, x0, need_to_extension
        #this case we only need to print 't2' and t1 += 4
        donot_need_to_extension:
            # load 32-bits instruction
            lw t4, 0(s6)
            # update ouput
            add t5, s1, s2
            sw t4, 0(t5)
            # update length
            add t5, s1, s3
            li t6, 4
            sw t6, 0(t5)
            # update loop variable
            addi s6, s6, 4
            addi s1, s1, 4
            j load_instr
    
        need_to_extension:
            # set arguments, load 16-bits instruction
            lhu a0, 0(s6)
            ## extension
            jal extend_instrucion
            addi t4, a0, 0
            # update ouput
            add t5, s1, s2
            sw t4, 0(t5)
            # update length
            add t5, s1, s3
            li t6, 2
            sw t6, 0(t5)
            # update loop variable
            addi s6, s6, 2
            addi s1, s1, 4
            j load_instr
    
    break_load_instr: li s1, 0

    adjust_offset:
        # check break
        slli t2, s4, 2
        bge s1, t2, break_adjust_offset

        add s8, s1, s2
        lw s8, 0(s8)
        # get opcode
        andi t2, s8, 0b1111111
        li t3, 0b1100011
        bne t2, t3, adjust_offset_type_J
        adjust_offset_type_B:
            li s7, 0
            # get sign bit
            srli t4, s8, 31
            andi t4, t4, 0b1
            beq t4, x0, adjust_B_positive
            # set imm[31:12]
            li t4, 0b11111111111111111111
            add s7, s7, t4
            adjust_B_positive:
            # imm[11]
            slli s7, s7, 1
            srli t3, s8, 7
            andi t3, t3, 0b1
            add s7, s7, t3
            
            # imm[10:5]
            slli s7, s7, 6
            srli t3, s8, 25
            andi t3, t3, 0b111111
            add s7, s7, t3

            # imm[4:1]
            slli s7, s7, 4
            srli t3, s8, 8
            andi t3, t3, 0b1111
            add s7, s7, t3

            # imm[0]
            slli s7, s7, 1

            # now s7 is the offset
            addi a0, s3, 0
            addi a1, s7, 0
            addi a2, s1, 0
            jal calculate_adjusted_offset
            # now a0 is the adjusted offset
            # reset imm
            li t2, 0b00000001111111111111000001111111
            and s8, s8, t2
            # imm[12]
            srli t1, a0, 12
            andi t1, t1, 0b1
            slli t1, t1, 31
            or s8, s8, t1

            # imm[10:5]
            srli t1, a0, 5
            andi t1, t1, 0b111111
            slli t1, t1, 25
            or s8, s8, t1

            # imm[4:1]
            srli t1, a0, 1
            andi t1, t1, 0b1111
            slli t1, t1, 8
            or s8, s8, t1

            # imm[11]
            srli t1, a0, 11
            andi t1, t1, 0b1
            slli t1, t1, 7
            or s8, s8, t1

            add t1, s1, s2
            sw s8, 0(t1)

            j end_of_loop
        adjust_offset_type_J:
            li t3, 0b1101111
            bne t2, t3, end_of_loop
            li s7, 0
            # get sign bit
            srli t4, s8, 31
            andi t4, t4, 0b1
            beq t4, x0, adjust_J_positive
            # imm[31:20]
            li t4, 0b111111111111
            add s7, s7, t4
            adjust_J_positive:
            # imm[19:12]
            slli s7, s7, 8
            srli t3, s8, 12
            andi t3, t3, 0b11111111
            add s7, s7, t3

            # imm[11]
            slli s7, s7, 1
            srli t3, s8, 20
            andi t3, t3, 0b1
            add s7, s7, t3

            # imm[10:1]
            slli s7, s7, 10
            srli t3, s8, 21
            andi t3, t3, 0b1111111111
            add s7, s7, t3

            # imm[0]
            slli s7, s7, 1

            # now s7 is the offset
            addi a0, s3, 0
            addi a1, s7, 0
            addi a2, s1, 0
            jal calculate_adjusted_offset
            # now a0 is the adjusted offset
            # reset imm
            li t2, 0b00000000000000000000111111111111
            and s8, s8, t2

            # imm[20]
            srli t1, a0, 20
            andi t1, t1, 0b1
            slli t1, t1, 31
            or s8, s8, t1

            # imm[10:1]
            srli t1, a0, 1
            andi t1, t1, 0b1111111111
            slli t1, t1, 21
            or s8, s8, t1

            # imm[11]
            srli t1, a0, 11
            andi t1, t1, 0b1
            slli t1, t1, 20
            or s8, s8, t1

            # imm[19:12]
            srli t1, a0, 12
            andi t1, t1, 0b11111111
            slli t1, t1, 12
            or s8, s8, t1

            add t1, s1, s2
            sw s8, 0(t1)
            
            j end_of_loop

        end_of_loop:
            addi s1, s1, 4
            j adjust_offset

    break_adjust_offset: li s1, 0
    output_loop:
        # check break
        slli t2, s4, 2
        bge s1, t2, end_of_main
        add a0, s1, s2
        lw a0, 0(a0)
        jal print_binary
        addi s1, s1, 4
        j output_loop
    
    end_of_main:
        # reset sp
        slli, t2, s4, 3
        add sp, sp, t2
        # exit with error code 0
        li a0, 17
        li a1, 0
        ecall
# print_binary(a0)
# a0 is the number you want to output
print_binary:
    addi t0, a0, 0
    li t1, 31
    loop_print_binary:
        blt t1, x0, end_of_print_binary
        srl t2, t0, t1
        andi t2, t2, 0b1
        addi t2, t2, '0'
        li a0, 11
        addi a1, t2, 0
        ecall
        addi t1, t1, -1
        j loop_print_binary
    end_of_print_binary:
        li a0, 11
        li a1, '\n'
        ecall
        ret

# calculate_adjusted_offset(a0, a1, a2)
# a0 is pointer of length of instruction
# a1 is the offset before adjusting
# a2 is the index of instruction * 4
calculate_adjusted_offset:
    li t0, 0
    addi t1, a2, 0
    addi t2, a1, 0
    blt a1, x0, loop_negative
    loop_positive:
        beq t2, x0, end_calculate_offset
        add t3, a0, t1
        lw t3, 0(t3)
        sub t2, t2, t3
        addi t0, t0, 4
        addi t1, t1, 4
        j loop_positive
    loop_negative:
        beq t2, x0, end_calculate_offset
        add t3, a0, t1
        addi t3, t3, -4
        lw t3, 0(t3)
        add t2, t2, t3
        addi t0, t0, -4
        addi t1, t1, -4
        j loop_negative
    end_calculate_offset:
        addi a0, t0, 0
        ret

# use s11
extend_instrucion: # (by Opcode)
    # save ra
    addi s11, ra, 0
    # CR type (ADD, MV, JR, JALR) and (SLLI) (CI type)
    andi t3, a0, 0b11
    beq t3, x0, CL_CS_Type
    addi t3, t3, -0b10
    bne t3, x0, CI_and_CA_and_CB_and_CJ_Type
    CRtype_and_CISLLI:
        #C.ADD_and_C.JALR
        srli t3, t2, 12
        addi t3, t3, -0b1001
        bne t3, x0, C_MV_and_C_JR
        ADD_and_JALR:
            C_JALR:
                srli t3, t2, 2
                andi t3, t3, 0b11111
                bne t3, x0, C_ADD
                jal JALR
                addi ra, s11, 0
                ret
            C_ADD:
                #C.ADD
                jal ADD
                addi ra, s11, 0
                ret
        C_MV_and_C_JR:
            # C.MV_and_C.JR
            srli t3, t2, 12
            addi t3, t3, -0b1000
            bne t3, x0, C_SLLI
            # C.MV_and_C.JR
            MV_and_JR:
                srli t3, t2, 2
                C_JR:
                    # JR
                    andi t3, t3, 0b11111
                    bne t3, x0, C_MV
                    jal JR
                    addi ra, s11, 0
                    ret
                C_MV:
                    # C_MV
                    jal MV
                    addi ra, s11, 0
                    ret
        C_SLLI:
            #SLLI
            jal SLLI
            addi ra, s11, 0
            ret
    CI_and_CA_and_CB_and_CJ_Type: 
        # CI type (LI, LUI, ADDI without SLLI) and CA type (AND, OR, XOR, SUB) and CB type (BEQZ, BNEZ, SRLI, SRAI, ANDI) and CJ type (J, JAL)
        andi t3, t2, 0b11
        addi t3, t3, -0b01
        bne t3, x0, CL_CS_Type
        CI_and_CA_and_CB_and_CJ:
            C_LI:
                # CI--C.LI (FUNCT3)
                srli t3, t2, 13
                addi t3, t3, -0b010
                bne t3, x0, C_LUI
                jal LI
                addi ra, s11, 0
                ret
            C_LUI:
                # CI--C.LUI (FUNCT3)
                srli t3, t2, 13
                addi t3, t3, -0b011
                bne t3, x0, C_ADDI
                jal LUI
                addi ra, s11, 0
                ret
            C_ADDI:
                # CI--C.ADDI (FUNCT3)
                srli t3, t2, 13
                bne t3, x0, CA_Types
                jal ADDI
                addi ra, s11, 0
                ret
            CA_Types:
                # CA (FUNCT6)
                srli t3, t2, 10
                addi t3, t3, -0b100011
                bne t3, x0, C_BEQZ
                CA:# switch by FUNCT2
                    C_AND:
                        # CA--C.AND
                        srli t3, t2, 5
                        andi t3, t3, 0b11
                        addi t3, t3, -0b11
                        bne t3, x0, C_OR
                        jal AND
                        addi ra, s11, 0
                        ret
                    C_OR:
                        # CA--C.OR
                        srli t3, t2, 5
                        andi t3, t3, 0b11
                        addi t3, t3, -0b10
                        bne t3, x0, C_XOR
                        jal OR
                        addi ra, s11, 0
                        ret
                    C_XOR:
                        # CA--C.XOR
                        srli t3, t2, 5
                        andi t3, t3, 0b11
                        addi t3, t3, -0b01
                        bne t3, x0, C_SUB
                        jal XOR
                        addi ra, s11, 0
                        ret
                    C_SUB:
                        # CA--C.SUB
                        srli t3, t2, 5
                        andi t3, t3, 0b11
                        bne t3, x0, Default
                        jal SUB
                        addi ra, s11, 0
                        ret
            C_BEQZ:
                # CB-type1--C.BEQZ(FUNCT3)
                srli t3, t2, 13
                addi t3, t3, -0b110
                bne t3, x0, C_BNEZ
                jal BEQZ
                addi ra, s11, 0
                ret
            C_BNEZ:
                # CB-type1--C.BNEZ(FUNCT3)
                srli t3, t2, 13
                addi t3, t3, -0b111
                bne t3, x0, CB2_Types
                jal BNEZ
                addi ra, s11, 0
                ret
            CB2_Types:
                # CB-type2 (FUNCT3)
                srli t3, t2, 13
                addi t3, t3, -0b100
                bne t3, x0, C_J
                CB_type2:
                    C_SRLI:
                        # CB-type2--C.SRLI
                        srli t3, t2, 10
                        andi t3, t3, 0b11
                        addi t3, t3, -0b00
                        bne t3, x0, C_SRAI
                        jal SRLI
                        addi ra, s11, 0
                        ret
                    C_SRAI:
                        # CA-type2--C.SRAI
                        srli t3, t2, 10
                        andi t3, t3, 0b11
                        addi t3, t3, -0b01
                        bne t3, x0, C_ANDI
                        jal SRAI
                        addi ra, s11, 0
                        ret
                    C_ANDI:
                        # CA-type2--C.ANDI
                        srli t3, t2, 10
                        andi t3, t3, 0b11
                        addi t3, t3, -0b10
                        bne t3, x0, Default
                        jal ANDI
                        addi ra, s11, 0
                        ret
            C_J:
                # CJ--C.J (FUNCT3)
                srli t3, t2, 13
                addi t3, t3, -0b101
                bne t3, x0, C_JAL
                jal J
                addi ra, s11, 0
                ret
            C_JAL:
                # CJ--C.JAL (FUNCT3)
                srli t3, t2, 13
                addi t3, t3, -0b001
                bne t3, x0, Default
                jal JAL
                addi ra, s11, 0
                ret
    CL_CS_Type:
        # CL and CS type
        andi t3, t2, 0b11
        addi t3, t3, -0b00
        bne t3, x0, Default
        CL_and_CS: #(LW and SW)
            C_LW:
                # CL--C.LW
                srli t3, t2, 13
                addi t3, t3, -0b010
                bne t3, x0, C_SW
                jal LW
                addi ra, s11, 0
                ret
            C_SW:
                # CS--C.SW
                srli t3, t2, 13
                addi t3, t3, -0b110
                bne t3, x0, Default
                jal SW
                addi ra, s11, 0
                ret
    Default:
        li a0, -1
        addi ra, s11, 0
        ret



ADD:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7
    addi t4, t4, 0x00 # ADD_FUNCT7 = 0x00
    # shift 5 bits for RS2
    slli t4, t4, 5
    # compute RS2
    srli t3, t2, 2
    andi t3, t3, 0b11111
    # RS2 (which compute in ADD_and_JALR)
    add t4, t4, t3
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # RS1
    add t4, t4, t3
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x0 # ADD_FUNCT3 = 0x0
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0110011
    addi a0, t4, 0
    jr ra
JALR:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # imm [11:5]
    addi t4, t4, 0
    # shift 5 bits for imm [4:0]
    slli t4, t4, 5
    # imm [4:0]
    addi t4, t4, 0
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RS1
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # RS1
    add t4, t4, t3
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x0 # JALR_FUNCT3 = 0x0
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    addi t4, t4, 1
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b1100111
    addi a0, t4, 0
    jr ra

MV:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7
    addi t4, t4, 0x00 # ADD_FUNCT7 = 0x00
    # shift 5 bits for RS2
    slli t4, t4, 5
    # compute RS2
    srli t3, t2, 2
    andi t3, t3, 0b11111
    # RS2 (which compute in ADD_and_JALR)
    add t4, t4, t3
    # shift 5 bits for RS1
    slli t4, t4, 5
    # RS1
    addi t4, t4, 0
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x0 # ADD_FUNCT3 = 0x0
    # shift 5 bits for RD
    slli t4, t4, 5
    # compute RD
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # RD
    add t4, t4, t3
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0110011
    addi a0, t4, 0
    jr ra
JR:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # imm [11:5]
    addi t4, t4, 0
    # shift 5 bits for imm [4:0]
    slli t4, t4, 5
    # imm [4:0]
    addi t4, t4, 0
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RS1
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # RS1
    add t4, t4, t3
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x0 # JALR_FUNCT3 = 0x0
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    addi t4, t4, 0
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b1100111
    addi a0, t4, 0
    jr ra
SLLI:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7 (imm[11:5])
    addi t4, t4, 0x00 # SLLI_FUNCT7 = 0x00 (imm[11:5])
    # shift 5 bits for imm[4:0]
    slli t4, t4, 5
    # imm[4:0]
    srli t3, t2, 2
    andi t3, t3, 0b11111
    add t4, t4, t3
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # RS1
    add t4, t4, t3
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x1 # SLLI_FUNCT3 = 0x1
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0010011
    addi a0, t4, 0
    jr ra


LI:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # imm[11:5]
    srli t3, t2, 12
    andi t3, t3, 0b1 #get imm[5]
    # compute sign bit
    srli t6, t2, 12
    andi t6, t6, 0b1    
    # imm[11:5]
    slli t4, t4, 7
    beq t6, x0, LI_positive
    addi t4, t4, 0b1111111
    LI_positive: addi t4, t4, 0b0000000
    # shift 5 bits for imm[4:0]
    slli t4, t4, 5
    # imm[4:0]
    srli t3, t2, 2
    andi t3, t3, 0b11111
    add t4, t4, t3
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # RS1
    addi t4, t4, 0
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x0 # ADD_FUNCT3 = 0x0
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0010011
    addi a0, t4, 0
    jr ra
LUI:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # compute sign bit
    srli t6, t2, 12
    andi t6, t6, 0b1    
    # imm[31:18]
    slli t4, t4, 14
    beq t6, x0, LUI_positive
    li t5,0b11111111111111
    add t4, t4, t5
    LUI_positive: addi t4, t4, 0b0
    # shift 4 bits for imm[17]
    slli t4, t4, 1
    # imm[17]
    srli t3, t2, 12
    andi t3, t3, 0b1 #get imm[17]
    add t4, t4, t3
    # shift 5 bits for imm[16:12]
    slli t4, t4, 5
    # imm[16:12]
    srli t3, t2, 2
    andi t3, t3, 0b11111 #get imm[16:12]
    add t4, t4, t3
    # compute RD
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0110111
    addi a0, t4, 0
    jr ra
ADDI:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # imm[11:5]
    # compute sign bit
    srli t6, t2, 12
    andi t6, t6, 0b1    
    # imm[11:5]
    slli t4, t4, 7
    beq t6, x0, ADDI_positive
    addi t4, t4, 0b1111111
    ADDI_positive: addi t4, t4, 0b0000000
    # shift 5 bits for imm[4:0]
    slli t4, t4, 5
    # imm[4:0]
    srli t3, t2, 2
    andi t3, t3, 0b11111
    add t4, t4, t3
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b11111
    # RS1
    add t4, t4, t3
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x0 # ADD_FUNCT3 = 0x0
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0010011
    addi a0, t4, 0
    jr ra

AND:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7
    addi t4, t4, 0x00 # ADD_FUNCT7 = 0x00
    # shift 5 bits for RS2
    slli t4, t4, 5
    # compute RS2
    srli t3, t2, 2
    andi t3, t3, 0b111
    # RS2
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x7 # AND_FUNCT3 = 0x7
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0110011
    addi a0, t4, 0
    jr ra
OR:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7
    addi t4, t4, 0x00 # OR_FUNCT7 = 0x00
    # shift 5 bits for RS2
    slli t4, t4, 5
    # compute RS2
    srli t3, t2, 2
    andi t3, t3, 0b111
    # RS2
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x6 # OR_FUNCT3 = 0x6
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0110011
    addi a0, t4, 0
    jr ra
XOR:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7
    addi t4, t4, 0x00 # XOR_FUNCT7 = 0x00
    # shift 5 bits for RS2
    slli t4, t4, 5
    # compute RS2
    srli t3, t2, 2
    andi t3, t3, 0b111
    # RS2
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x4 # XOR_FUNCT3 = 0x4
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0110011
    addi a0, t4, 0
    jr ra
SUB:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7
    addi t4, t4, 0x20 # SUB_FUNCT7 = 0x20
    # shift 5 bits for RS2
    slli t4, t4, 5
    # compute RS2
    srli t3, t2, 2
    andi t3, t3, 0b111
    # RS2
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x0 # SUB_FUNCT3 = 0x0
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0110011
    addi a0, t4, 0
    jr ra
BEQZ:
    addi t2, a0, 0
    # compute sign bit
    srli t6, t2, 12
    andi t6, t6, 0b1
    # init t4
    li t4, 0
    
    # imm[12|10:9]
    slli t4, t4, 3
    beq t6, x0, BEQZ_positive
    addi t4, t4, 0b111
    BEQZ_positive: addi t4, t4, 0b000

    # imm[8]
    slli t4, t4, 1
    # compute imm[8]
    srli t3, t2, 12
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[7:6]
    slli t4, t4, 2
    # compute imm[7:6]
    srli t3, t2, 5
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[5]
    slli t4, t4, 1
    # compute imm[5]
    srli t3, t2, 2
    andi t3, t3, 0b1
    add t4, t4, t3

    # rs2
    slli t4, t4, 5

    # rs1
    slli t4, t4, 5
    # compute rs1'
    srli t3, t2, 7
    andi t3, t3, 0b111
    add t4, t4, t3
    addi t4, t4, 0b1000

    # funct3
    slli t4, t4, 3
    addi t4, t4, 0b000

    # imm[4:3]
    slli t4, t4, 2
    # compute imm[4:3]
    srli t3, t2, 10
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[2:1]
    slli t4, t4, 2
    # compute imm[2:1]
    srli t3, t2, 3
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[11]
    slli t4, t4, 1
    beq t6, x0, BEQZ_positive2
    addi t4, t4, 0b1
    BEQZ_positive2: addi t4, t4, 0b0
    # opcode
    slli t4, t4, 7
    addi t4, t4, 0b1100011

    addi a0, t4, 0
    ret
BNEZ:
    addi t2, a0, 0
    # compute sign bit
    srli t6, t2, 12
    andi t6, t6, 0b1
    # init t4
    li t4, 0
    
    # imm[12|10:9]
    slli t4, t4, 3
    beq t6, x0, BNEZ_positive
    addi t4, t4, 0b111
    BNEZ_positive: addi t4, t4, 0b000

    # imm[8]
    slli t4, t4, 1
    # compute imm[8]
    srli t3, t2, 12
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[7:6]
    slli t4, t4, 2
    # compute imm[7:6]
    srli t3, t2, 5
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[5]
    slli t4, t4, 1
    # compute imm[5]
    srli t3, t2, 2
    andi t3, t3, 0b1
    add t4, t4, t3

    # rs2
    slli t4, t4, 5

    # rs1
    slli t4, t4, 5
    # compute rs1'
    srli t3, t2, 7
    andi t3, t3, 0b111
    add t4, t4, t3
    addi t4, t4, 0b1000

    # funct3
    slli t4, t4, 3
    addi t4, t4, 0b001

    # imm[4:3]
    slli t4, t4, 2
    # compute imm[4:3]
    srli t3, t2, 10
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[2:1]
    slli t4, t4, 2
    # compute imm[2:1]
    srli t3, t2, 3
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[11]
    slli t4, t4, 1
    beq t6, x0, BNEZ_positive2
    addi t4, t4, 0b1
    BNEZ_positive2: addi t4, t4, 0b0
    # opcode
    slli t4, t4, 7
    addi t4, t4, 0b1100011

    addi a0, t4, 0
    ret
SRLI:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7 (imm[11:5])
    addi t4, t4, 0x00 # SRLI_FUNCT7 = 0x00 (imm[11:5])
    # shift 5 bits for imm[4:0]
    slli t4, t4, 5
    # imm[4:0]
    srli t3, t2, 2
    andi t3, t3, 0b11111
    add t4, t4, t3
    #shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x5 # SRLI_FUNCT3 = 0x5
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0010011
    addi a0, t4, 0
    jr ra
SRAI:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # FUNCT7 (imm[11:5])
    addi t4, t4, 0x20 # SRAI_FUNCT7 = 0x20 (imm[11:5])
    # shift 5 bits for imm[4:0]
    slli t4, t4, 5
    # imm[4:0]
    srli t3, t2, 2
    andi t3, t3, 0b11111
    add t4, t4, t3
    #shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x5 # SRAI_FUNCT3 = 0x5
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0010011
    addi a0, t4, 0
    jr ra
ANDI:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # imm[11:5]
    srli t3, t2, 12
    andi t3, t3, 0b1 #get imm[5]
    #imm[11]
    add t4, t4, t3
    slli t4, t4, 1
    #imm[10]
    add t4, t4, t3
    slli t4, t4, 1
    #imm[9]
    add t4, t4, t3
    slli t4, t4, 1
    #imm[8]
    add t4, t4, t3
    slli t4, t4, 1
    #imm[7]
    add t4, t4, t3
    slli t4, t4, 1
    #imm[6]
    add t4, t4, t3
    slli t4, t4, 1
    #imm[5]
    add t4, t4, t3
    # slli t4, t4, 1
    # shift 5 bits for imm[4:0]
    slli t4, t4, 5
    # imm[4:0]
    srli t3, t2, 2
    andi t3, t3, 0b11111
    add t4, t4, t3
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RD/RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x7 # ANDI_FUNCT3 = 0x7
    # shift 5 bits for RD
    slli t4, t4, 5
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0010011
    addi a0, t4, 0
    jr ra
J:
    addi t2, a0, 0
    # compute sign bit
    srli t6, t2, 12
    andi t6, t6, 0b1
    # init t4
    li t4, 0
    # imm[20]
    slli t4, t4, 1
    beq t6, x0, J_positive_1
    addi t4, t4, 0b1
    J_positive_1: addi t4, t4, 0b0

    # imm[10]
    slli t4, t4, 1
    # compute imm[10]
    srli t3, t2, 8
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[9:8]
    slli t4, t4, 2
    # compute imm[9:8]
    srli t3, t2, 9
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[7]
    slli t4, t4, 1
    # compute imm[7]
    srli t3, t2, 6
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[6]
    slli t4, t4, 1
    # compute imm[6]
    srli t3, t2, 7
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[5]
    slli t4, t4, 1
    # compute imm[5]
    srli t3, t2, 2
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[4]
    slli t4, t4, 1
    # compute imm[4]
    srli t3, t2, 11
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[3:1]
    slli t4, t4, 3
    # compute imm[3:1]
    srli t3, t2, 3
    andi t3, t3, 0b111
    add t4, t4, t3

    # imm[11]
    slli t4, t4, 1
    # compute imm[11]
    srli t3, t2, 12
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[19:12]
    slli t4, t4, 8
    beq t6, x0, J_positive_2
    addi t4, t4, 0b11111111
    J_positive_2: addi t4, t4, 0b00000000

    # RD, x0
    slli t4, t4, 5
    addi t4, t4, 0

    # opcode
    slli t4, t4, 7
    addi t4, t4 0b1101111

    addi a0, t4, 0
    ret
JAL:
    addi t2, a0, 0
    # compute sign bit
    srli t6, t2, 12
    andi t6, t6, 0b1
    # init t4
    li t4, 0
    # imm[20]
    slli t4, t4, 1
    beq t6, x0, JAL_positive_1
    addi t4, t4, 0b1
    JAL_positive_1: addi t4, t4, 0b0

    # imm[10]
    slli t4, t4, 1
    # compute imm[10]
    srli t3, t2, 8
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[9:8]
    slli t4, t4, 2
    # compute imm[9:8]
    srli t3, t2, 9
    andi t3, t3, 0b11
    add t4, t4, t3

    # imm[7]
    slli t4, t4, 1
    # compute imm[7]
    srli t3, t2, 6
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[6]
    slli t4, t4, 1
    # compute imm[6]
    srli t3, t2, 7
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[5]
    slli t4, t4, 1
    # compute imm[5]
    srli t3, t2, 2
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[4]
    slli t4, t4, 1
    # compute imm[4]
    srli t3, t2, 11
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[3:1]
    slli t4, t4, 3
    # compute imm[3:1]
    srli t3, t2, 3
    andi t3, t3, 0b111
    add t4, t4, t3

    # imm[11]
    slli t4, t4, 1
    # compute imm[11]
    srli t3, t2, 12
    andi t3, t3, 0b1
    add t4, t4, t3

    # imm[19:12]
    slli t4, t4, 8
    beq t6, x0, JAL_positive_2
    addi t4, t4, 0b11111111
    JAL_positive_2: addi t4, t4, 0b00000000

    # RD
    slli t4, t4, 5
    addi t4, t4, 1

    # opcode
    slli t4, t4, 7
    addi t4, t4 0b1101111

    addi a0, t4, 0
    ret
LW:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # imm[11:7]
    slli t4, t4, 5
    # shift 1+3+1 bits for imm[6:2]
    # shift 1 bits for imm[6]
    slli t4, t4, 1
    srli t3, t2, 5
    andi t3, t3, 0b1
    add t4, t4, t3
    # shift 3 bits for imm[5:3]
    slli t4, t4, 3
    srli t3, t2, 10
    andi t3, t3, 0b111
    add t4, t4, t3
    # shift 1 bits for imm[2]
    slli t4, t4, 1
    srli t3, t2, 6
    andi t3, t3, 0b1
    add t4, t4, t3
    # shift 2 bits for imm[1:0]
    slli t4, t4, 2
    addi t4, t4, 0
    # shift 5 bits for RS1
    slli t4, t4, 5
    # compute RS1
    srli t3, t2, 7
    andi t3, t3, 0b111
    # RS1
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 3 bits for FUNCT3
    slli t4, t4, 3
    # FUNCT3
    addi t4, t4, 0x2 # LW_FUNCT3 = 0x2
    # shift 5 bits for RD
    slli t4, t4, 5
    # compute RD
    srli t3, t2, 2
    andi t3, t3, 0b111
    # RD
    add t4, t4, t3
    addi t4, t4, 0b1000
    # shift 7 bits for Opcode
    slli t4, t4, 7
    # Opcode
    addi t4, t4, 0b0000011
    addi a0, t4, 0
    jr ra
SW:
    addi t2, a0, 0
    #init t4
    li t4, 0
    # imm[11:7] is not sign-extended
    slli t4, t4, 5
    # comupte imm[6]
    # imm[6]
    slli t4, t4, 1
    # comupte imm[6]
    srli t3, t2, 5
    andi t3, t3, 0b1
    add t4, t4, t3
    # imm[5]
    slli t4, t4, 1
    # compute imm[5]
    srli t3, t2, 12
    andi t3, t3, 0b1
    add t4, t4, t3
    # rs2
    slli t4, t4, 5
    # compute rs2
    srli t3, t2, 2
    andi t3, t3, 0b111
    add t4, t4, t3
    addi t4, t4, 0b1000
    # rs1
    slli t4, t4, 5
    # compute rs1
    srli t3, t2, 7
    andi t3, t3, 0b111
    add t4, t4, t3
    addi t4, t4, 0b1000
    # funct3
    slli t4, t4, 3
    addi t4, t4, 0b010
    # imm[4:3]
    slli t4, t4, 2
    # compute imm[4:3]
    srli t3, t2, 10
    andi t3, t3, 0b11
    add t4, t4, t3
    # imm[2]
    slli t4, t4, 1
    # compute imm[2]
    srli t3, t2, 6
    andi t3, t3, 0b1
    add t4, t4, t3
    # imm[1:0]
    slli t4, t4, 2
    # opcode
    slli t4, t4, 7
    addi t4, t4, 0b0100011
    addi a0, t4, 0
    ret
