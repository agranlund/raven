
#include <portab.h>
#include <gembind.h>


	WORD
wind_update(beg_update)
	WORD		beg_update;
{
	WM_BEGUP = beg_update;
	return( crys_if( WIND_UPDATE ) );
}
