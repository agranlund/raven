
extern setjmp(), longjmp();

#ifndef LINT

/*
	setjmp -- establish a jump buffer
*/
asm {
	setjmp:
			move.l  4(A7),A0
			move.l  (A7),A1
			movem.l	A1-A3/A6-A7/D4-D7, 4(A0)
			clr.l 	D0
			rts
}


/*
	longjmp -- restore control to a jump buffer
*/
asm {
	longjmp:
			move.l	4(A7),A0
			move	8(A7),D0
			bne		notscrewed
			moveq	#1,D0
	notscrewed:
			movem.l	4(A0), A1-A3/A6-A7/D4-D7
			move.l 	A1,(A7)
			rts
}
#endif
