
#include <portab.h>
#include <gembind.h>

	WORD
scrp_write(pscrap)
	LONG		pscrap;
{
	SC_PATH = pscrap;
	return( crys_if( SCRP_WRITE ) );
}

