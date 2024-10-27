#include "gem_aesP.h"

_WORD _mt_aes_alt(MX_PARMDATA *data, const _WORD *control, _WORD *aes_global)
{
	AESPB pb;
	
	pb.control = data->control;
	data->control[0] = control[0]; /* opcode */
	data->control[1] = control[1]; /* nintin */
	data->control[2] = control[2]; /* nintout */
	data->control[3] = control[3]; /* naddrin */
	data->control[4] = 0;          /* naddrout */
	pb.global = aes_global;
	if (pb.global == NULL)
		pb.global = _GemParBlk.global;
	pb.intin = data->intin;
	pb.intout = data->intout;
	pb.addrin = data->addrin;
	pb.addrout = data->addrout;
	return aes(&pb);
}
