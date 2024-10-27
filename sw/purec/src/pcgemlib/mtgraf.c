#include "gem_aesP.h"

_WORD mt_graf_mkstate_event(EVNTDATA *data, _WORD *global_aes)
{
	_WORD ret;
	
	AES_PARAMS(79,0,5,0,0);
	ret = AES_TRAP(aes_params);
	*data = *((EVNTDATA *)&aes_intout[1]);
	return ret;
}
