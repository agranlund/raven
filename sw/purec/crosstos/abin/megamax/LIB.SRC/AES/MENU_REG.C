
#include <portab.h>
#include <gembind.h>


	WORD
menu_register(pid, pstr)
	WORD		pid;
	LONG		pstr;
{
	MM_PID = pid;
	MM_PSTR = pstr;
	return( crys_if( MENU_REGISTER ) );
}

