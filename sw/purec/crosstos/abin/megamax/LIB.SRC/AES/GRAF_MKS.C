
#include <portab.h>
#include <gembind.h>


	WORD
graf_mkstate(pmx, pmy, pmstate, pkstate)
	WORD		*pmx, *pmy, *pmstate, *pkstate;
{
	crys_if( GRAF_MKSTATE );
	*pmx = GR_MX;
	*pmy = GR_MY;
	*pmstate = GR_MSTATE;
	*pkstate = GR_KSTATE;
}
