
/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

#ifdef	lint
int strlen(s)
char *s;
{
	register char *base = s;

	while (*s++);

	return s - base - 1;
}
#else
extern int strlen();

asm {
strlen:
		move.l	4(A7), A0
		moveq	#-1, D0
		bra.s	loop
adjust:	addq.w	#1, D0
		subq.l	#1, D0
loop:	tst.b	(A0)+
		dbeq	D0, loop
		bne.s	adjust
		not.l	D0
		rts
}
#endif
