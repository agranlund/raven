
#include <portab.h>
#include <gembind.h>

 
/*
	Plays a piece of a GEM AES recording of the user's actions.
*/

	WORD
appl_tplay(tbuffer, tlength, tscale)
	LONG		tbuffer;
	WORD		tlength;
	WORD		tscale;
{
	AP_TBUFFER = tbuffer;
	AP_TLENGTH = tlength;
	AP_TSCALE = tscale;
	return( crys_if(APPL_TPLAY) );
}
