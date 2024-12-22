
#include <portab.h>
#include <gembind.h>

					/* Shell Manager		*/
	WORD
shel_read(pcmd, ptail)
	LONG		pcmd, ptail;
{
	SH_PCMD = pcmd;
	SH_PTAIL = ptail;
	return( crys_if( SHEL_READ ) );
}
