
#include <portab.h>
#include <gembind.h>

/*
	Records a set of user interactions.
*/

WORD appl_trecord(tbuffer, tlength)
	LONG		tbuffer;
	WORD		tlength;
{
	AP_TBUFFER = tbuffer;
	AP_TLENGTH = tlength;
	return( crys_if(APPL_TRECORD) );
}

