
#include <portab.h>
#include <gembind.h>


	WORD
form_center(tree, pcx, pcy, pcw, pch)
	LONG		tree;
	WORD		*pcx, *pcy, *pcw, *pch;
{
	FM_FORM = tree;
	crys_if(FORM_CENTER);
	*pcx = FM_XC;
	*pcy = FM_YC;
	*pcw = FM_WC;
	*pch = FM_HC;
	return( RET_CODE );
}
