
#include <portab.h>
#include <gembind.h>



	WORD
shel_find(ppath)
	LONG		ppath;
{
	SH_PATH = ppath;
	return( crys_if( SHEL_FIND ) );
}

