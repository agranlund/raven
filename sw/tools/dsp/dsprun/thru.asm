	include "dsp56303.inc"

	org p:$0
	jmp main

	org p:M_START

main:

	jclr	#M_RDF,x:<<M_SSISR0,*	; read left
	movep	x:<<M_RX0,a1

    jclr    #M_TDE,x:<<M_SSISR0,*	; write left
    movep   a1,x:<<M_TX00

	jclr	#M_RDF,x:<<M_SSISR0,*	; read right
	movep	x:<<M_RX0,b1

    jclr    #M_TDE,x:<<M_SSISR0,*	; write right
    movep   b1,x:<<M_TX00

    jmp     <main
