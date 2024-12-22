
#include <portab.h>
#include <gembind.h>

					/* Scrap Manager		*/
	WORD
scrp_read(pscrap)
	LONG		pscrap;
{
	SC_PATH = pscrap;
	return( crys_if( SCRP_READ ) );
}
