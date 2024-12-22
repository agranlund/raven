
#include <vdibind.h>

    WORD
v_dspcur( handle, x, y )
WORD handle;		/* Physical device handle */
WORD x;
WORD y;
{
    ptsin[0] = x;
    ptsin[1] = y;

    contrl[0] = 5;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[5] = 18;
    contrl[6] = handle;
    vdi();
}
