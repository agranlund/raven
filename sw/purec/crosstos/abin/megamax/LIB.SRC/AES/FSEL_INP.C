
#include <portab.h>
#include <gembind.h>



					/* fseler Manager		*/
	WORD
fsel_input(pipath, pisel, pbutton)
	LONG		pipath, pisel;
	WORD		*pbutton;
{
	FS_IPATH = pipath;
	FS_ISEL = pisel;
	crys_if( FSEL_INPUT );
	*pbutton = FS_BUTTON;
	return( RET_CODE );
}

