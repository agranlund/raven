#include "gem_vdiP.h"

/** is an extended version of vqt_name()
 *
 *  @param handle Device handle
 *  @param index index (1 - number of fonts)
 *  @param name 
 *         - name[0..31] : font name
 *         - name[32] :  0: bitmap font, 1: vector font
 *  @param font_format 
 *         - 1: bitmap font
 *         - 2: Speedo font
 *         - 4: TrueType font
 *         - 8: Type 1 font
 *  @param flags 
 *         - 0: proportional font
 *         - 1: monospaced font
 *
 *  @return font id or 0 on failure
 *
 *  @since NVDI 3.00
 *
 *
 *
 */

_WORD vqt_ext_name(_WORD handle, _WORD index, char *name, _WORD *font_format, _WORD *flags)
{
	_WORD vdi_control[VDI_CNTRLMAX]; 
	_WORD vdi_intin[2];   
	_WORD vdi_intout[35]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	vdi_intin[0] = index;
	vdi_intin[1] = 0;

	/* set the 0 as return value in case NVDI is not present */
	vdi_intout[0] = 0;

	VDI_TRAP_ESC (vdi_params, handle, 130,1, 0,2);

	vdi_array2str (vdi_intout + 1, name, 32);
	if (vdi_control[4] > 34)
	{
		name[32]     = vdi_intout[33];
		*flags       = (vdi_intout[34] >> 8) & 0xff;
		*font_format = vdi_intout[34] & 0xff;
	}
	else if (vdi_control[4] > 33 )
	{
		name[32]	 = vdi_intout[33];
		*flags		 = 0;
		*font_format = vdi_intout[33] ? 0 : 1;
	}
	else
	{
		name[32] 	 = 0;
		*flags		 = 0;
		*font_format = 0;
	}
	
	return vdi_intout[0];
}
