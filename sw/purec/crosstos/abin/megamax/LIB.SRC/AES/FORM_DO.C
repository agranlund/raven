
#include <portab.h>
#include <gembind.h>


					/* Form Manager			*/
	WORD
form_do(form, start)
	LONG		form;
	WORD		start;
{
	FM_FORM = form;
	FM_START = start;
	return( crys_if( FORM_DO ) );
}
