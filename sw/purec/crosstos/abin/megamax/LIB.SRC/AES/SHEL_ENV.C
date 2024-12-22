
#include <portab.h>
#include <gembind.h>


	WORD
shel_envrn(ppath, psrch)
	LONG		ppath;
	LONG		psrch;
{
	SH_PATH = ppath;
	SH_SRCH = psrch;
	return( crys_if( SHEL_ENVRN ) );
}


