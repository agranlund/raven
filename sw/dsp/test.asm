	include "dsp56303.inc"
	org 	p:$0
	jmp		start

	org		p:$100
start:
	bset	#3,x:M_HCR				; signal HF2
    move    #0,a                	; sawtooth counter

loop:
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       	; output left sample 
	nop								; delay a bit so M_SSISR0 has
	nop								; time to update status
    jclr    #M_TDE,x:<<M_SSISR0,*
    movep   a1,x:<<M_TX00       	; output right sample
    add     #$008000,a
	bchg	#4,x:M_HCR				; toggle HF3
    jmp     loop
