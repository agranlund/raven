#include "gem_aesP.h"

_WORD mt_rsrc_gaddr(WORD type, WORD index, void *o, WORD *global_aes)
{
	_WORD ret;
	AES_PARAMS(112,2,1,0,1);
	
	aes_intin[0] = type;
	aes_intin[1] = index;
	
	aes_addrout[0] = 0;
	ret = AES_TRAP(aes_params);
	*((void **)o) = aes_addrout[0];
	return ret;
}


_WORD mt_rsrc_load(const char *Name, _WORD *global_aes)
{
	AES_PARAMS(110,0,1,1,0);
                    
	aes_addrin[0] = NO_CONST(Name);

	return AES_TRAP(aes_params);
}


_WORD mt_rsrc_free( _WORD *global_aes)
{
	AES_PARAMS(111,0,1,0,0);

	return AES_TRAP(aes_params);
}


_WORD mt_rsrc_obfix(OBJECT *Tree, _WORD Index, _WORD *global_aes)
{
	AES_PARAMS(114,1,1,1,0);
                    
	aes_intin[0]  = Index;
	aes_addrin[0] = Tree;

	return AES_TRAP(aes_params);
}


_WORD mt_rsrc_rcfix(void *rc_header, _WORD *global_aes)
{
	AES_PARAMS(115,0,1,1,0);
                    
	aes_addrin[0] = rc_header;

	return AES_TRAP(aes_params);
}


_WORD mt_rsrc_saddr(_WORD Type, _WORD Index, void *Address, _WORD *global_aes)
{
	AES_PARAMS(113,2,1,1,0);
                    
	aes_intin[0]  = Type;
	aes_intin[1]  = Index;
	aes_addrin[0] = Address;

	return AES_TRAP(aes_params);
}
