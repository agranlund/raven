/* LINTLIBRARY */
/*
	Absolute value
*/

abs(i)
int i;
{
#ifdef	lint
	return i >= 0 ? i : -i;
#else
	asm {
		move.w	i(A6), D0
		bge.s	exit
		neg.w	D0
	exit:
	}
#endif
}

