#include "gem_vdiP.h"
#include "mt_gemx.h"

#undef max
#define max(x,y)    (((x)>(y))?(x):(y))

_WORD vdi_dummy[max(max(max(max(VDI_CNTRLMAX, VDI_INTINMAX), VDI_INTOUTMAX), VDI_PTSOUTMAX), VDI_PTSINMAX)];

void v_get_driver_info(_WORD device, _WORD select, char *info_string)
{
	_WORD vdi_control[VDI_CNTRLMAX]; 
	_WORD vdi_intin[VDI_INTINMAX];   
	_WORD vdi_intout[VDI_INTOUTMAX];   
	char *bptr;
	int i;
	
	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy );
	
	vdi_intin[0] = device;
	vdi_intin[1] = select;

	VDI_TRAP_ESC (vdi_params, 0, -1,4, 0,2);
	*info_string = 0;
	if (vdi_control[4] != 0)
	{
		switch (select)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			bptr = (char *) vdi_intout;
			for (i = 0; i < vdi_control[4]; i++)
				*info_string++ = *bptr++;
			*info_string = '\0';
			break;
		case 5:
			*((short *)info_string) = vdi_intout[0];
			break;
		}
	}
}


void v_set_app_buff(_WORD handle, void **buf_p, _WORD size)
{
	_WORD vdi_control[VDI_CNTRLMAX]; 
	_WORD vdi_intin[3];   

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_dummy, vdi_dummy );
	
	vdi_intin_ptr(0) = buf_p;
	vdi_intin    [2] = size;
	
	VDI_TRAP_ESC (vdi_params, handle, -1,6, 0,3);
}
