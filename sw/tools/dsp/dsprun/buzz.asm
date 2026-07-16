	include "dsp56303.inc"

	org p:$0
	jmp start		; $00 reset

	org p:M_START
start:
	clr		a
	clr		b
	
loop:
	nop
	nop
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       	; output left sample 
    add     #$004000,a

	nop
	nop
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   b1,x:<<M_TX00       	; output right sample
	sub		#$004000,b
    jmp     loop
