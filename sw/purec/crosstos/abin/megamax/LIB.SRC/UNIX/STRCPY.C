/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

#ifdef	lint
char *strcpy(s1, s2)
char *s1, *s2;
{
	register char *s = s1;

	while (*s1++ = *s2++);
	return s;
}
#else
extern char *strcpy();

asm {
strcpy:
		movem.l	4(A7), A0/A1
		move.l	A0, D0
		moveq	#-1, D1
loop:
		move.b	(A1)+, (A0)+
		dbeq	D1, loop
		bne.s	loop
		rts
}
#endif
