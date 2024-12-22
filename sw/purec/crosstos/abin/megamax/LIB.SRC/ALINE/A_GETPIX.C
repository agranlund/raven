
#include <linea.h>

extern lineaport *_lnaport;

int a_getpixel(x, y)
	 int x, y;
{
	asm {
#ifdef PCREL
		    move.l  _lnaport(A4), A0
#else
		    move.l  _lnaport, A0
#endif
			move.l  lineaport.ptsin(A0), A0
			move.w  x(A6), (A0)+
			move.w  y(A6), (A0)

			movem.l D2/A2, -(A7)
			dc.w    Lna_GETPIXEL
			movem.l (A7)+, D2/A2
	}
}
