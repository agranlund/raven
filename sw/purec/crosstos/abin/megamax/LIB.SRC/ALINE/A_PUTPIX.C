
#include <linea.h>

extern lineaport *_lnaport;

int a_putpixel(x, y, color)
	 int x, y;
	 int color;
{
	asm {
#ifdef PCREL
		    move.l _lnaport(A4), A0
#else
		    move.l _lnaport, A0
#endif

			move.l  lineaport.ptsin(A0), A1
			move.w  x(A6), (A1)+
			move.w  y(A6), (A1)
			move.l  lineaport.intin(A0), A1
			move.w  color(A6), (A1)

			movem.l D2/A2, -(A7)
			dc.w    Lna_PUTPIXEL
			movem.l (A7)+, D2/A2
	}
}
