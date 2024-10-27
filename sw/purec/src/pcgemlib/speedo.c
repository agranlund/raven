#include "gem_vdiP.h"

#if 0 /* in vescape.s */
_WORD vq_margins(_WORD handle, _WORD *top, _WORD *bot, _WORD *lft, _WORD *rgt, _WORD *xdpi, _WORD *ydpi)
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[7];

	VDI_PARAMS (vdi_control, vdi_dummy, vdi_dummy, vdi_intout, vdi_dummy);
	
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2100, 0, 0);
	
	*top = vdi_intout[1];
	*bot = vdi_intout[2];
	*lft = vdi_intout[3];
	*rgt = vdi_intout[4];
	*xdpi = vdi_intout[5];
	*ydpi = vdi_intout[6];
	return vdi_intout[0];
}
#endif

_WORD vq_driver_info(_WORD handle, _WORD *lib, _WORD *drv, _WORD *plane, _WORD *attr, char name[27])
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[31];

	VDI_PARAMS (vdi_control, vdi_dummy, vdi_dummy, vdi_intout, vdi_dummy);
	
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2101, 0, 0);
	
	*lib = vdi_intout[1];
	*drv = vdi_intout[2];
	*plane = vdi_intout[3];
	*attr = vdi_intout[4];	
	vdi_array2str(&vdi_intout[5], name, 26);
	return vdi_intout[0];
}

_WORD vq_bit_image(_WORD handle, _WORD *ver, _WORD *maximg, _WORD *form)
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[4];

	VDI_PARAMS (vdi_control, vdi_dummy, vdi_dummy, vdi_intout, vdi_dummy);
	
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2102, 0, 0);
	
	*ver = vdi_intout[1];
	*maximg = vdi_intout[2];
	*form = vdi_intout[3];
	return vdi_intout[0];
}

_WORD vs_page_info(_WORD handle, _WORD type, const char txt[60])
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[1];
	_WORD vdi_intin[62];
	short i;

	VDI_PARAMS (vdi_control, vdi_intin, vdi_dummy, vdi_intout, vdi_dummy);
	
	vdi_intin[0] = type;
	i = vdi_str2arrayn(txt, &vdi_intin[1], 60) + 1;
	vdi_intin[i++] = '\0';
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2103, 0, i);
	
	return vdi_intout[0];
}

_WORD vs_crop(_WORD handle, _WORD ltx1, _WORD lty1, _WORD ltx2, _WORD lty2, _WORD ltlen, _WORD ltoffset)
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[2];
	_WORD vdi_intin[6];

	VDI_PARAMS (vdi_control, vdi_intin, vdi_dummy, vdi_intout, vdi_dummy);
	
	vdi_intin[0] = ltx1;
	vdi_intin[1] = lty1;
	vdi_intin[2] = ltx2;
	vdi_intin[3] = lty2;
	vdi_intin[4] = ltlen;
	vdi_intin[5] = ltoffset;	
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2104, 0, 6);
	
	return vdi_intout[0];
}


_WORD vq_image_type(_WORD handle, const char *file, BIT_IMAGE *image)
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[2];
	_WORD vdi_intin[VDI_INTINMAX];
	_WORD vdi_ptsin[N_PTRINTS];
	short i;

	VDI_PARAMS (vdi_control, vdi_intin, vdi_ptsin, vdi_intout, vdi_dummy);
	
	i = vdi_str2array(file, vdi_intin);
	vdi_intin[i++] = 0;
	((long *)(vdi_ptsin))[0] = (long)image;
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2105, 1, i);
	
	return vdi_intout[0];
}


_WORD vs_save_disp_list(_WORD handle, const char *name)
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[2];
	_WORD vdi_intin[N_PTRINTS];

	VDI_PARAMS (vdi_control, vdi_intin, vdi_dummy, vdi_intout, vdi_dummy);
	
	((long *)(vdi_intin))[0] = (long)name;
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2106, 0, 2);
	
	return vdi_intout[0];
}


_WORD vs_load_disp_list(_WORD handle, const char *name)
{
	_WORD vdi_control[VDI_CNTRLMAX];
	_WORD vdi_intout[2];
	_WORD vdi_intin[N_PTRINTS];

	VDI_PARAMS (vdi_control, vdi_intin, vdi_dummy, vdi_intout, vdi_dummy);
	
	((long *)(vdi_intin))[0] = (long)name;
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2107, 0, 2);
	
	return vdi_intout[0];
}
