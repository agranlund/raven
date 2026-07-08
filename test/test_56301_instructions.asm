    org p:$0
    
    bra >test
    bra <test
    bra r0
    bra r7
    beq >test
    beq <test
    beq r0
    bne r7
    bsr >test
    bsr <test
    bsr r0
    bsr r7
    nop
    
test:
    nop
    
    
