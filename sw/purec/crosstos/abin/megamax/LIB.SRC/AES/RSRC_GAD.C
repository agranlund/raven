
#include <portab.h>
#include <gembind.h>



	WORD
rsrc_gaddr(rstype, rsid, paddr)
	WORD		rstype;
	WORD		rsid;
	LONG		*paddr;
{
	RS_TYPE = rstype;
	RS_INDEX = rsid;
	control[4] = 1;
	crys_if(RSRC_GADDR);
	control[4] = 0;
	*paddr = RS_OUTADDR;
	return( RET_CODE );
}
