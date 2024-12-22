
#include <portab.h>
#include <gembind.h>


	WORD
wind_get(w_handle, w_field, pw1, pw2, pw3, pw4)
	WORD		w_handle;
	WORD		w_field;
	WORD		*pw1, *pw2, *pw3, *pw4;
{
	WM_HANDLE = w_handle;
	WM_WFIELD = w_field;
	crys_if( WIND_GET );
	*pw1 = WM_OX;
	*pw2 = WM_OY;
	*pw3 = WM_OW;
	*pw4 = WM_OH;
	return( RET_CODE );
}
