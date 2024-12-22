
#include <vdibind.h>

    WORD
v_meta_extents( handle, min_x, min_y, max_x, max_y )
WORD handle;		/* Physical device handle */
WORD min_x;
WORD min_y;
WORD max_x;
WORD max_y;
{
    ptsin[0] = min_x;
    ptsin[1] = min_y;
    ptsin[2] = max_x;
    ptsin[3] = max_y;

    contrl[0] = 5;
    contrl[1] = 2;
    contrl[3] = 0;
    contrl[5] = 98;
    contrl[6] = handle;
    vdi();
}
