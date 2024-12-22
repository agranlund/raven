
_blockmv()
/* 
    Block move routine. Address of block to move from goes in A1. Destination
    address goes in A0. D0 contains the number of words to move minus 1.
    Contents of A1 are not changed. Contents of A0 and D0 are destroyed.
*/
{
#ifndef LINT
    asm {
		move.l	A1, -(A7)			; save A1
	lp:	move	(A1)+, (A0)+
		dbf		D0, lp
		move.l	(A7)+, A1			;restore A1
    }
#endif
}
