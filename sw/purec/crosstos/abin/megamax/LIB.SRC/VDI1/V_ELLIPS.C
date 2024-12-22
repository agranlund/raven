
#include <vdibind.h>

    WORD
v_ellipse( handle, xc, yc, xrad, yrad )
WORD handle;		/* Physical device handle */
WORD xc;
WORD yc;
WORD xrad;
WORD yrad;
{
    ptsin[0] = xc;
    ptsin[1] = yc;
    ptsin[2] = xrad;
    ptsin[3] = yrad;

    contrl[0] = 11;
    contrl[1] = 2;
    contrl[3] = 0;
    contrl[5] = 5;
    contrl[6] = handle;
    vdi();
}
