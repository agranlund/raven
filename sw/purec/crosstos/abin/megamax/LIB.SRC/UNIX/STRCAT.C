/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

char *strcat(s1,s2)
char *s1, *s2;
{
#ifdef	lint
    register char *result = s1;

    for ( ; *s1++; ) ;
    --s1;
	while (*s1++ = *s2++);
    return result;
#else
	asm {
		move.l	s1(A6), D0
		move.l	D0, A0
		move.l	s2(A6), A1
		moveq	#-1, D1
eloop:	tst.b	(A0)+
		dbeq	D1, eloop
		bne.s	eloop
		subq.l	#1, A0
cloop:	move.b	(A1)+, (A0)+
		dbeq	D1, cloop
		bne.s	cloop
	}
#endif
}
