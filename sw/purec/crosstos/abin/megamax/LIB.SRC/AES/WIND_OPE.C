
#include <portab.h>
#include <gembind.h>


	WORD
wind_open(handle, wx, wy, ww, wh)
	WORD		handle;
	WORD		wx, wy, ww, wh;
{
	WM_HANDLE = handle;
	WM_WX = wx;
	WM_WY = wy;
	WM_WW = ww;
	WM_WH = wh;
	return( crys_if( WIND_OPEN ) );
}

