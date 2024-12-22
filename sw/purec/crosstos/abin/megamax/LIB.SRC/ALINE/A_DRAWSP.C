
#include <linea.h>

a_drawsprite(x, y, thesprite, thebackgnd)
	 int x, y;
	 sprite     *thesprite;
	 spriteback *thebackgnd;
{
	asm {
		    move.w x(A6), D0
			move.w y(A6), D1
			move.l thesprite(A6), A0
			move.l thebackgnd(A6), A2

			movem.l D2/A2/A6, -(A7)
			dc.w    Lna_DRAWSPRITE
			movem.l (A7)+, D2/A2/A6
	}
}
