
#include <linea.h>
#include <gemdefs.h>

extern lineaport *_lnaport;

a_copyraster(source, destin)
	 MFDB      *source, *destin;
{
	asm {
			movem.l D2/A2, -(A7)
#ifdef PCREL
		    move.l _lnaport(A4), A0
#else
		    move.l _lnaport, A0
#endif

			move.l lineaport.cntrl(A0), A0
			move.l source(A6), 0x0e(A0)
			move.l destin(A6), 0x12(A0)
			dc.w   Lna_COPYRASTER

			movem.l (A7)+, D2/A2
	}
}
