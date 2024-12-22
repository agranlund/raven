
#include <portab.h>
#include <gembind.h>

	WORD
shel_get(pbuffer, len)
	LONG		pbuffer;
	WORD		len;
{
	SH_PBUFFER = pbuffer;
	SH_LEN = len;
	return( crys_if( SHEL_GET ) );
}
