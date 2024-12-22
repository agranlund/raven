
/* LINTLIBRARY */
#include <strings.h>
#include <stdio.h>

#ifdef	lint
char *index(s, c)
char *s;
int c;
{
    register int temp;

    for ( ; (temp = *s++) && temp != c; ) ;
    return temp ? &s[-1] : 0;
}
#else
extern char *index();

asm {
index:
		move.l	4(A7), A0
		move.b	9(A7), D1
		bne.s	nonzero
		moveq	#-1, D0
loop:	tst.b	(A0)+
		dbeq	D0, loop
		bne.s	loop
		bra.s	ret
nonzero:
		move.b	(A0)+, D0
		beq.s	notfound
		cmp.b	D0, D1
		bne.s	nonzero
ret:	subq.l	#1, A0
		move.l	A0, D0
		bra.s	exit
notfound:
		moveq	#0, D0
exit:
		rts
}
#endif
