
#include <portab.h>
#include <gembind.h>



	WORD
evnt_button(clicks, mask, state, pmx, pmy, pmb, pks)
	WORD		clicks;
	UWORD		mask;
	UWORD		state;
	WORD		*pmx, *pmy, *pmb, *pks;
{
	B_CLICKS = clicks;
	B_MASK = mask;
	B_STATE = state;
	crys_if(EVNT_BUTTON);
	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	return(RET_CODE);
}
