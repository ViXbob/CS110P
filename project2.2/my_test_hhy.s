.data
test_input: .word 3 6 7 8

.text
main:
	add t0, x0, x0
	addi t1, x0, 4
	la t2, test_input
main_loop:
	beq t0, t1, main_exit
	slli s0, t0, 2
	add s1, t2, s0
	lw a0, 0(s1)
	addi sp, x0, 400
	sw t0, 0(sp)
	sw t1, 4(sp)
	sw t2, 8(sp)
	sw s0, 12(sp)
	sw s1, 16(sp)
	jal ra, factorial
	lw t0, 0(sp)
	lw t1, 4(sp)
	lw t2, 8(sp)
	lw s0, 12(sp)
	lw s1, 16(sp)
	addi t0, t0, 1
	jal x0, main_loop
factorial:
	# YOUR CODE HERE
	addi t0, a0, 0
	addi t1, x0, 1
	loop:
		addi t0, t0, -1
		mul a0, a0,t0
		bne t0, t1, loop
	ret
main_exit:  
	addi a0, x0, 0