lui sp, 240
addi sp, sp, -1

addi sp, sp, -256

li t0, 0x12345678
li t1, 0x23456789
li t2, 0x34567890

sw t0, 0(sp)
sw t1, 4(sp)
sw t2, 8(sp)

lw s0, 0(sp)
lw t0, 4(sp)
lw t1, 8(sp)
