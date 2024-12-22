
#include <vdibind.h>

    WORD
v_pieslice( handle, xc, yc, rad, sang, eang )
WORD handle;		/* Physical device handle */
WORD xc;
WORD yc;
WORD rad;
WORD sang;
WORD eang;
{
    ptsin[0] = xc;
    ptsin[1] = yc;
    ptsin[2] = 0;
    ptsin[3] = 0;
    ptsin[4] = 0;
    ptsin[5] = 0;
    ptsin[6] = rad;
    ptsin[7] = 0;
    intin[0] = sang;
    intin[1] = eang;

    contrl[0] = 11;
    contrl[1] = 4;
    contrl[3] = 2;
    contrl[5] = 3;
    contrl[6] = handle;
    vdi();
}
