
#include <portab.h>
#include <gembind.h>


	WORD
evnt_mesag(pbuff)
	LONG		pbuff;
{
	ME_PBUFF = pbuff;
	return( crys_if(EVNT_MESAG) );
}
