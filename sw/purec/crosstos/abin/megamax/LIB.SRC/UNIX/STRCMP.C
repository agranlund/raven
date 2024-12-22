/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

#ifdef	lint
int strcmp(s1, s2)
char *s1, *s2;
{
	char temp;
	for (; (temp = -*s2++) && !(temp += *s1++); );
	return temp;
}
#else
extern int strcmp();

asm {
strcmp:
		movem.l	4(A7), A0/A1
		clr.w	D0
loop:
		move.b	(A0)+, D0
		beq.s	exit
		cmp.b	(A1)+, D0
		beq.s	loop
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
