
#include <portab.h>
#include <gembind.h>

/*
Exits a session with the Application library.  This call allows the AES to 
clean up the applications environment.
*/

	UWORD
appl_exit()
{
	crys_if(APPL_EXIT);
	return( TRUE );
}
