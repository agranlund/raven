
#include <linea.h>

lineaport *_lnaport;
fontform  **_fonthdrs;

/*
    linea_init() - Trap $A000
	return values
	    A0 & D0 - pointer to linea port base.
		A1      - pointer to font header records.
		A2      - pointer to table of linea functions
*/
#ifdef PCREL
lineaport *a_init()
{
	asm {
		    dc.w    Lna_INIT
			move.l  D0, _lnaport(A4)
			move.l  A1, _fonthdrs(A4)
	}
}
#else
lineaport *a_init()
{
	asm {
		    dc.w    Lna_INIT
			move.l  D0, _lnaport
			move.l  A1, _fonthdrs
	}
}
#endif
