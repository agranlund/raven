/* LINTLIBRARY */
/*
	Absolute value (long)
*/

long labs(l)
long l;
{
#ifdef	lint
	return l >= 0 ? l : -l;
#else
	asm {
		move.l	l(A6), D0
		bge.s	exit
		neg.l	D0
	exit:
	}
#endif
}
