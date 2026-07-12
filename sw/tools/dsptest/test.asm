	include "dsp56303.inc"

	org p:$0
	jmp start		; $00 reset

	org p:M_START
start:
	clr		a
loop:
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       	; output left sample 
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       	; output right sample
    add     #$004000,a
    jmp     loop
