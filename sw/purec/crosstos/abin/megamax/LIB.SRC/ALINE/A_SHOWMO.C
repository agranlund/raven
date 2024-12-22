
#include <linea.h>

a_showmouse()
{
	asm {
			movem.l D2/A2, -(A7)
		    dc.w    Lna_SHOWMOUSE
			movem.l (A7)+, D2/A2
	}
}
