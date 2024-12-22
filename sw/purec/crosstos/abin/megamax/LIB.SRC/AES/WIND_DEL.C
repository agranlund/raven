
#include <portab.h>
#include <gembind.h>



	WORD
wind_delete(handle)
	WORD		handle;
{
	WM_HANDLE = handle;
	return( crys_if( WIND_DELETE ) );
}

