
#include <portab.h>
#include <gembind.h>


	WORD
shel_write(doex, isgr, iscr, pcmd, ptail)
	WORD		doex, isgr, iscr;
	LONG		pcmd, ptail;
{
	SH_DOEX = doex;
	SH_ISGR = isgr;
	SH_ISCR = iscr;
	SH_PCMD = pcmd;
	SH_PTAIL = ptail;
	return( crys_if( SHEL_WRITE ) );
}
