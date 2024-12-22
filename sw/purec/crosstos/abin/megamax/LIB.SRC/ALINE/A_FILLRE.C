
#include <linea.h>

extern  lineaport *_lnaport;

a_fillrect(x1, y1, x2, y2)
	 int x1, y1, x2, y2;
{
	asm {
#ifdef PCREL
		    move.l _lnaport(A4), A0
#else
		    move.l _lnaport, A0
#endif
			lea    lineaport.x1(A0), A0
			move.w x1(A6), (A0)+
			move.w y1(A6), (A0)+
			move.w x2(A6), (A0)+
			move.w y2(A6), (A0)

			movem.l D2/A2, -(A7)
			dc.w    Lna_FILLRECT
			movem.l (A7)+, D2/A2
	}
}
