
#include <portab.h>
#include <gembind.h>



					/* Window Manager		*/
	WORD
wind_create(kind, wx, wy, ww, wh)
	UWORD		kind;
	WORD		wx, wy, ww, wh;
{
	WM_KIND = kind;
	WM_WX = wx;
	WM_WY = wy;
	WM_WW = ww;
	WM_WH = wh;
	return( crys_if( WIND_CREATE ) );
}

