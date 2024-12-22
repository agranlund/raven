/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

#ifdef	lint
char *xtrcpy(s1, s2)
char *s1, *s2;
{
	while (*s1++ = *s2++);
	return s1 - 1;
}
#else
extern char *xtrcpy();

asm {
xtrcpy:
		movem.l	4(A7), A0/A1
		moveq	#-1, D1
loop:
		move.b	(A1)+, (A0)+
		dbeq	D1, loop
		bne.s	loop

		move.l	A0, D0
		subq.l	#1, D0
		rts
}
#endif
