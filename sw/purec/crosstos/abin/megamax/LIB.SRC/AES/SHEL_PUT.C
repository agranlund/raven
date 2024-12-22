
#include <portab.h>
#include <gembind.h>

	WORD
shel_put(pdata, len)
	LONG		pdata;
	WORD		len;
{
	SH_PDATA = pdata;
	SH_LEN = len;
	return( crys_if( SHEL_PUT ) );
}
