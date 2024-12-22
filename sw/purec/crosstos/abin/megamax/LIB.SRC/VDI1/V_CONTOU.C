
#include <vdibind.h>

    WORD
v_contourfill( handle, x, y, index )
WORD handle;		/* Physical device handle */
WORD x;
WORD y;
WORD index;
{
    intin[0] = index;
    ptsin[0] = x;
    ptsin[1] = y;

    contrl[0] = 103;
    contrl[1] = 1;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
}
