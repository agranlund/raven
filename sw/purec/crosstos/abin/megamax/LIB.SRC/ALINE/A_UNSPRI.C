
#include <linea.h>

a_undrawsprite(thebackgnd)
	 spriteback *thebackgnd;
{
	asm {
		    move.l  thebackgnd(A6), A2
			movem.l D2/A2/A6, -(A7)
			dc.w    Lna_UNSPRITE
			movem.l (A7)+, D2/A2/A6
	}
}
