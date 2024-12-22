
#include <portab.h>
#include <gembind.h>

/*
Finds the application identifier (ap_id) of another application. The 
application must pass APPL_FIND the address of a null-terminated string 
containing the filename of the application for which the current application 
is searching.
*/

WORD appl_find(pname)
	LONG		pname;
{						
	AP_PNAME = pname;			
	return( crys_if(APPL_FIND) );
}
