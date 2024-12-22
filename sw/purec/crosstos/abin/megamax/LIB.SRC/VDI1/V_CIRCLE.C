
#include <vdibind.h>

    WORD
v_circle( handle, xc, yc, rad )
WORD handle;		/* Physical device handle */
WORD xc;
WORD yc;
WORD rad;
{
    ptsin[0] = xc;
    ptsin[1] = yc;
    ptsin[2] = 0;
    ptsin[3] = 0;
    ptsin[4] = rad;
    ptsin[5] = 0;

    contrl[0] = 11;
    contrl[1] = 3;
    contrl[3] = 0;
    contrl[5] = 4;
    contrl[6] = handle;
    vdi();
}
