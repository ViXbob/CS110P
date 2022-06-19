main: 
    addi t1,x0,3
    addi t0,x0,3
    beq t0,t1,label2
    label2: 
        addi t1,t1,1
    addi t1,t1,2
    auipc s0,123
    lui s1,123