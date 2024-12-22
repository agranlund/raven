
#include <portab.h>
#include <gembind.h>


	WORD
evnt_dclick(rate, setit)
	WORD		rate, setit;
{
	EV_DCRATE = rate;
	EV_DCSETIT = setit;
	return( crys_if(EVNT_DCLICK) );
}
