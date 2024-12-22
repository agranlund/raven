
#include <linea.h>

a_bitblit(theblock)
	 blitblock *theblock;
{
	asm {
		    movem.l  D2/A2/A6, -(A7)
			move.l	theblock(A6), A6
		    dc.w    Lna_BITBLIT
			movem.l  (A7)+, D2/A2/A6
	}
}
