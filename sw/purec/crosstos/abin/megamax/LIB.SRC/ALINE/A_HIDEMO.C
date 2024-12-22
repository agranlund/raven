
#include <linea.h>

a_hidemouse()
{
	asm {
			movem.l D2/A2, -(A7)
		    dc.w    Lna_HIDEMOUSE
			movem.l (A7)+, D2/A2
	}
}
