addi sp, sp, 80
addi t0, x0, 1
sw t0, 0(sp)
sw t0, 4(sp)
addi s0, x0, 40
addi t2, sp, 8
loop:
    addi t0, t2, -8
    lw t0, 0(t0)
    addi t1, t2, -4
    lw t1, 0(t1)
    add t0, t0, t1
    sw t0, 0(t2)
    addi t2, t2, 4
    blt t2, s0, loop