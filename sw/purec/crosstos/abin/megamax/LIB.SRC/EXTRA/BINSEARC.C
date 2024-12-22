extern _Binsrch();

asm {
_Binsrch:
		move	D1, -(A7)
		clr		D2
loop:
		move	D1, D3		;D3 is the guess its (D1 + D2)>>1
		add		D2, D3
		lsr		#2, D3
		asl		#1, D3
		cmp		0(A1, D3), D0
		beq		foundit
		bcc		bigger
		move	D3, D1
		bra		test
bigger:
		move	D3, D2
		addq	#2, D2
test:
		cmp		D1, D2
		bne		loop
		move	(A7)+, D3
		add		D3, D3
		bra		exit
foundit:
		move	(A7)+, D1
		add		D1, D3
exit:
		adda	0(A1, D3), A0
		rts
}


