cmb_g       equ    0.86
lpf1        equ    0.408
fdbck1        equ    cmb_g*(1.0-lpf1)

        ORG    P:40
        movec    #>3999,m0 ; You dont have to fix this coz > have
						; no meaning for m-regs but I had it in
						; my code so... do as you please.
        move    #'ABC',x0 ; Would be nice...
        move    #>$80,x0  ; But this is really important!

        ORG X:0
lp_states:    dc    1.0-lpf1
        ORG    Y:$1600
udlyln:        dsm    300    ; Why is this ridiculous? :-) 