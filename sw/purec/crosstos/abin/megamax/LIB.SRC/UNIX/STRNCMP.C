/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

/*
 * Predecrement because on the last character, no test is needed.
 */

#ifdef	lint
int strncmp(s1, s2, n)
register char *s1, *s2;
register int n;
{
    while (--n && *s1 && *s1 == *s2) {
		++s1; ++s2;
    }
    return *s1 - *s2;
}
#else
extern int strncmp();

asm {
strncmp:
		movem.l	4(A7),A0/A1
		move.w  12(A7), D1
		bne.s	cont
		move.w	D1, D0
		rts
cont:
		subq.w	#1, D1
		clr.w	D0
loop:
		move.b	(A0)+, D0
		beq.s	exit
		cmp.b	(A1)+, D0
		dbne	D1, loop
		subq.l	#1, A1
exit:
		clr.w	D1
		move.b	(A1), D1
		beq.s	noD0extend
		ext.w	D0
noD0extend:
		tst.w	D0
		beq.s	noD1extend
		ext.w	D1
noD1extend:
		sub.w	D1, D0
		rts
}
#endif
