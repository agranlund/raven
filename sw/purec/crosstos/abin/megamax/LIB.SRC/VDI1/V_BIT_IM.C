
#include <vdibind.h>

    WORD
v_bit_image( handle, filename, aspect, x_scale, y_scale, h_align, v_align, xy)
WORD handle;		/* Physical device handle */
BYTE *filename;
WORD aspect;
WORD x_scale, y_scale;
WORD h_align, v_align;
WORD xy[4];
/* EP 1-14-87; Changes made due to bug report from customer 
	    scale changed to x_scale, y_scale, h_align, v_align
	    num_pts removed */
{
    WORD i;

    for (i = 0; i < 4; i++)
	ptsin[i] = xy[i];
    intin[0] = aspect;
    intin[1] = x_scale;
    intin[2] = y_scale;
    intin[3] = h_align;
    intin[4] = v_align;
    
    for (i=5; intin[i] = *filename++; i++)
	;

    contrl[0] = 5;
    contrl[1] = 2;
    contrl[2] = 0;
    contrl[3] = --i;
    contrl[5] = 23;
    contrl[6] = handle;
    vdi();
}
