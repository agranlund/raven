
#include <linea.h>

extern lineaport *_lnaport;

a_transformmouse(newform)
	 mouse *newform;
{
	asm {
#ifdef PCREL
			move.l _lnaport(A4), A0
#else
			move.l _lnaport, A0
#endif

			move.l lineaport.intin(A0), A0
			move.l newform(A6), A1

			moveq	#17, D0
	loop:
		    move.l   (A1)+,(A0)+					
			dbf	     D0, loop

			movem.l D2/A2, -(A7)
			dc.w    Lna_NEWMOUSE
			movem.l (A7)+, D2/A2
	}
}
