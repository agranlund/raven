
#include <portab.h>
#include <gembind.h>


	VOID
graf_rubberbox(xorigin, yorigin, wmin, hmin, pwend, phend)
	WORD		xorigin, yorigin;
	WORD		wmin, hmin;
	WORD		*pwend, *phend;
{
	GR_I1 = xorigin;
	GR_I2 = yorigin;
	GR_I3 = wmin;
	GR_I4 = hmin;
	crys_if( GRAF_RUBBOX );
	*pwend = GR_O1;
	*phend = GR_O2;
	return( RET_CODE );
}

