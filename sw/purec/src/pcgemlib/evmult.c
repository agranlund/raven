#include "gem_aesP.h"

void  mt_EVNT_multi(_WORD evtypes, _WORD nclicks, _WORD bmask, _WORD bstate,
	const MOBLK *m1, const MOBLK *m2, unsigned long ms, EVNT *event, _WORD *global_aes)
{
	MOBLK *m;
	
	AES_PARAMS(25,16,7,1,0);
	
	aes_intin[0] = evtypes;
	aes_intin[1] = nclicks;
	aes_intin[2] = bmask;
	aes_intin[3] = bstate;
	
	if (evtypes & MU_M1)
	{
		m = (MOBLK *)(aes_intin + 4);
		*m = *m1;
	}
	
	if (evtypes & MU_M2)
	{
		m = (MOBLK *)(aes_intin + 9);
		*m = *m2;
	}
	
	aes_intin[14] = (_WORD)ms;
	aes_intin[15] = (_WORD)(ms >> 16);
	
	aes_addrin[0] = event->msg;
	
	AES_TRAP(aes_params);
	
	event->mwhich = aes_intout[0];
	event->mx = aes_intout[1];
	event->my = aes_intout[2];
	event->mbutton = aes_intout[3];
	event->kstate = aes_intout[4];
	event->key = aes_intout[5];
	event->mclicks = aes_intout[6];
}

void (EVNT_multi)(_WORD evtypes, _WORD nclicks, _WORD bmask, _WORD bstate,
	const MOBLK *m1, const MOBLK *m2, unsigned long ms, EVNT *event)
{
	mt_EVNT_multi(evtypes, nclicks, bmask, bstate, m1, m2, ms, event, aes_global);
}