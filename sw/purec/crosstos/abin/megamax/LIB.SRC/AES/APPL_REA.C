
#include <portab.h>
#include <gembind.h>

/*
Lets an application read aspecified number of bytes from the message pipe.
*/

WORD appl_read(rwid, length, pbuff)
	WORD		rwid;
	WORD		length;
	LONG		pbuff;
{
	AP_RWID = rwid;
	AP_LENGTH = length;
	AP_PBUFF = pbuff;
	return( crys_if(APPL_READ) );
}

