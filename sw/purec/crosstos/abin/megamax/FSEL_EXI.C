
#include <portab.h>
#include <gembind.h>



					/* fseler Manager		*/
/*	New for TOS 1.4
*/
	WORD
fsel_exinput(pipath, pisel, pbutton, plabel)
	LONG		pipath, pisel;
	WORD		*pbutton;
	LONG		plabel;		/**	Actually (char*)	**/
{
	FS_IPATH = pipath;
	FS_ISEL = pisel;
	FS_ILABEL	= plabel;
	crys_if( FSEL_EXINPUT );
	*pbutton = FS_BUTTON;
	return( RET_CODE );
}

