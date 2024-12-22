
#include <portab.h>
#include <gembind.h>


	WORD
evnt_timer(locnt, hicnt)
	UWORD		locnt, hicnt;
{
	T_LOCOUNT = locnt;
	T_HICOUNT = hicnt;
	return( crys_if(EVNT_TIMER) );
}
