
/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

#ifdef	lint
char *rindex(s, c)
char *s;
int c;
{
    register char *p = 0;
    register char temp;

	while (temp = *s++)
		if (temp == c)
			p = &s[-1];
    return p;
}
#else
extern char *rindex();

asm {
rindex:
		move.l	4(A7), A0
		move.b	9(A7), D2
		bne.s	nonzero
		moveq	#-1, D0
loop:	tst.b	(A0)+
		dbeq	D0, loop
		bne.s	loop
		move.l	A0, D0
		bra.s	ret
nonzero:
		move.l	#1, D0
nloop:	
		move.b	(A0)+, D1
		beq.s	ret
		cmp.b	D1, D2
		bne.s	nloop
		move.l	A0, D0
		bra.s	nloop
ret:	
		subq.l	#1, D0
		rts
}
#endif

