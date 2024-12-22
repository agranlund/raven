
#include <portab.h>
#include <gembind.h>


UWORD evnt_keybd()
{
	return( crys_if(EVNT_KEYBD) );
}

