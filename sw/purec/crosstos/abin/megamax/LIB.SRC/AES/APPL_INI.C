
#include <portab.h>
#include <gembind.h>

/*
Initializes a session with the Application Library. You must use this call 
first in order to use the AES or VDI functions.
*/

	UWORD
appl_init()
{
	WORD		i;		/* Junk word not used */

	_c.cb_pcontrol = (&control[0]);
	_c.cb_pglobal = (&global[0]);
	_c.cb_pintin = (&int_in[0]);
	_c.cb_pintout = (&int_out[0]);	
	_c.cb_padrin = (&addr_in[0]);
	_c.cb_padrout = (&addr_out[0]);
	_ad_c = (&_c);
	crys_if(APPL_INIT);
	gl_apid = RET_CODE;
	return( gl_apid );
}
