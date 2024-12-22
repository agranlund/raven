
#include <portab.h>
#include <gembind.h>


	WORD
rsrc_saddr(rstype, rsid, lngval)
	WORD		rstype;
	WORD		rsid;
	LONG		lngval;
{
	RS_TYPE = rstype;
	RS_INDEX = rsid;
	RS_INADDR = lngval;
	return( crys_if(RSRC_SADDR) );
}

