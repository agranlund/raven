
#include <portab.h>
#include <gembind.h>



	WORD
form_error(errnum)
	WORD		errnum;
{
	FM_ERRNUM = errnum;
	return( crys_if( FORM_ERROR ) );
}
