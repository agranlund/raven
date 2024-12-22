
#include <portab.h>
#include <gembind.h>



	WORD
wind_close(handle)
	WORD		handle;
{
	WM_HANDLE = handle;
	return( crys_if( WIND_CLOSE ) );
}

