
#include <portab.h>
#include <gembind.h>


	WORD
wind_find(mx, my)
	WORD		mx, my;
{
	WM_MX = mx;
	WM_MY = my;
	return( crys_if( WIND_FIND ) );
}
