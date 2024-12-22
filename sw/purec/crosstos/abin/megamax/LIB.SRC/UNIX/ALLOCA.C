/*	ALLOCA
	- Allocate on stack
*/
extern char *alloca();
asm {
alloca:
	MOVEA.L	(A7)+, A0
	MOVE.L	A7, D0
	MOVE.W	(A7), D1
	EXT.L	D1
	SUB.L	D1, D0
	AND.L	#0xfffffffc, D0
	MOVEA.L	D0, A7
	TST.B	0(A7)
	ADDQ.L	#2, D0
	JMP  	(A0)
}

