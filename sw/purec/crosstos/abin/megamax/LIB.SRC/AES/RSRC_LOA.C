
#include <portab.h>
#include <gembind.h>


	WORD
rsrc_load(rsname)
	LONG	rsname;
{
	RS_PFNAME = rsname;
	return( crys_if(RSRC_LOAD) );
}
