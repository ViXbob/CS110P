.section .text
.globl IO_init
.type IO_init, @function
IO_init:
	# molloc
	addi sp, sp, -4
	# save ra
	sw ra, 0(sp)
	#inport init
	jal Inp_init
	# A/D init
	jal Adc_init
	# LCD init
	jal Lcd_Init
	# load ra
	lw ra, 0(sp)
	addi sp, sp, 4
	# return 
	ret