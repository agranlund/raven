/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

char *xtrcat(s1,s2)
char *s1, *s2;
{
#ifdef	lint
    for ( ; *s1++; ) ;
    --s1;
	while (*s1++ = *s2++);
    return s1 - 1;
#else
	asm {
		move.l	s1(A6), A0
		move.l	s2(A6), A1
		moveq	#-1, D0
eloop:	tst.b	(A0)+
		dbeq	D0, eloop
		bne.s	eloop
		subq.l	#1, A0
cloop:	move.b	(A1)+, (A0)+
		dbeq	D0, cloop
		bne.s	cloop
		move.l	A0, D0
		subq.l	#1, D0
	}
#endif
}
