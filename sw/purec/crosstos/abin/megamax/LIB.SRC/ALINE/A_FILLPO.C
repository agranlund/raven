
#include <linea.h>

extern lineaport *_lnaport;

a_fillpoly(vert, points, numpts)
	 int vert, *points, numpts;
{
	asm {
#ifdef PCREL
		    move.l  _lnaport(A4), A0
#else
		    move.l  _lnaport, A0
#endif

			move.l  lineaport.ptsin(A0), -(A7)
			move.l  points(A6), lineaport.ptsin(A0)
			move.w  vert(A6), lineaport.y1(A0)
			move.l  lineaport.cntrl(A0), A0
			move.l  numpts(A6), 0x02(A0)

			movem.l D2/A2, -(A7)
			dc.w    Lna_FILLPOLY
			movem.l (A7)+, D2/A2

#ifdef PCREL
		    move.l  _lnaport(A4), A0
#else
		    move.l  _lnaport, A0
#endif
			move.l  (A7)+, lineaport.ptsin(A0)
	}
}
