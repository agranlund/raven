
#include <linea.h>

extern lineaport *_lnaport;

a_textblit(charblock)
	 textblock *charblock;
{
	_lnaport -> thetext = *charblock;
	asm {
			movem.l D2/A2, -(A7)
			dc.w    Lna_TEXTBLIT
			movem.l (A7)+, D2/A2
	}
}
