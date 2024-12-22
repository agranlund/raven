/* LINTLIBRARY */

bzero(b, len)
char *b;
int len;
{
#ifdef	lint
	while (--len >= 0)
		*b++ = 0;
#else
	asm {
		move.l	b(A6), A0
		move.w	len(A6), D0
		bra		eloop
loop:	clr.b	(A0)+
eloop:	dbf		D0, loop
	}
#endif
}

