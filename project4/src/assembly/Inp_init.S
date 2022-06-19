.section .text
.globl Inp_init
.type Inp_init, @function
Inp_init:
	# molloc
	addi sp, sp, -4
	# save ra
	sw ra, 0(sp)
	# input parameters
	# GPIOA = GPIO_BASE + 0x00000000U = (APB2_BUS_BASE + 0x00000800U)
	li a0, 0x40010800
	# GPIO_MODE_IN_FLOATING
	li a1, 0x04
	# GPIO_OSPEED_50MHZ
	li a2, 0x03
	# GPIO_PIN_8 = BIT(8)=256
	li a3, 256
	# call func gpio_init
	jal gpio_init
	#load ra
	lw ra, 0(sp)
	addi sp, sp, 4
	# return 
	ret