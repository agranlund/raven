
#include <portab.h>
#include <gembind.h>


	WORD
form_alert(defbut, astring)
	WORD		defbut;
	LONG		astring;
{
	FM_DEFBUT = defbut;
	FM_ASTRING = astring;
	return( crys_if( FORM_ALERT ) );
}
