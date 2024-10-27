#include "gem_aesP.h"
#include "winframe.h"

#define global_aes NULL

typedef void AES_FUNCTION(AESPB *pb);

void sys_set_getdisp(void **disp_addr, void **disp_err)
{
	AES_PARAMS(0, 1, 0, 0, 2);

	aes_intin[0] = 0;	/* Subcode 0: get AES dispatcher */
	aes_addrout[0] = 0;
	aes_addrout[1] = 0;
	AES_TRAP(aes_params);
	*disp_addr = aes_addrout[0];
	if (disp_err)
		*disp_err = aes_addrout[1];
}


AES_FUNCTION *sys_set_getfn(WORD fn)
{
	AES_PARAMS(0, 2, 0, 0, 1);

	aes_intin[0] = 1;	/* Subcode 1: get AES function */
	aes_intin[1] = fn;	/* function number */
	aes_addrout[0] = 0;
	AES_TRAP(aes_params);
	return aes_addrout[0];
}


WORD sys_set_setfn(WORD fn, AES_FUNCTION *f)
{
	AES_PARAMS(0, 2, 1, 1, 0);

	aes_intin[0] = 1;	/* Subcode 2: set AES function */
	aes_intin[1] = fn;	/* function number */
	aes_addrin[0] = f;
	aes_intout[0] = 0;
	return AES_TRAP(aes_params);
}


AES_FUNCTION *sys_set_appl_getinfo(AES_FUNCTION *f)
{
	AES_PARAMS(0, 1, 0, 1, 1);

	aes_intin[0] = 3;	/* Subcode 3: change appl_getinfo */
	aes_addrin[0] = f;
	aes_addrout[0] = 0;
	AES_TRAP(aes_params);
	return aes_addrout[0];
}


void *sys_set_editob(WORD __CDECL (*editob)(PARMBLK *pb))
{
	AES_PARAMS(0, 1, 0, 1, 1);

	aes_intin[0] = 4;	/* Subcode 4: define edit object */
	aes_addrin[0] = editob;	/* Funktionsnummer */
	aes_addrout[0] = 0;
	AES_TRAP(aes_params);
	return aes_addrout[0];
}


void sys_set_colortab(const WORD *colortab)
{
	AES_PARAMS(0, 1, 0, 1, 0);

	aes_intin[0] = 5;	/* Subcode 5: change color table */
	aes_addrin[0] = colortab;
	AES_TRAP(aes_params);
}


WORD sys_set_winframe_manager(WINFRAME_HANDLER *old_wfh, WINFRAME_HANDLER *new_wfh, WINFRAME_SETTINGS **set)
{
	AES_PARAMS(0, 1, 1, 2, 1);

	aes_intin[0] = 6;	/* Subcode 6: change window frame manager */
	aes_addrin[0] = old_wfh;
	aes_addrin[1] = new_wfh;
	aes_addrout[0] = 0;
	AES_TRAP(aes_params);
	if (set)
		*set = aes_addrout[0];
	return aes_intout[0];
}
