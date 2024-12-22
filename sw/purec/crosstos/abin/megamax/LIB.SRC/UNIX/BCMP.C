/* LINTLIBRARY */

bcmp(b1, b2, len)
char *b1, *b2;
int len;
{
#ifdef	lint
	while (--len >= 0)
		if (*b1++ != *b2++)
			return 0;
	return 1;
#else
	asm {
		move.l	b1(A6), A0
		move.l	b2(A6), A1
		move.w	len(A6), D1
		move.w	#0, D0
		bra		eloop
loop:	cmpm.b	(A0)+,(A1)+
eloop:	dbne	D1, loop
		beq		done
		move.w	#1, D0
done:	
	}
#endif
}
