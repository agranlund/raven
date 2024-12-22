
#include <portab.h>
#include <gembind.h>

/*
Lets an application write a specified number of bytes to the message pipe.
*/


WORD appl_write(rwid, length, pbuff)	
	WORD		rwid;
	WORD		length;
	LONG		pbuff;
{
	AP_RWID = rwid;
	AP_LENGTH = length;
	AP_PBUFF = pbuff;
	return( crys_if(APPL_WRITE) );
}
