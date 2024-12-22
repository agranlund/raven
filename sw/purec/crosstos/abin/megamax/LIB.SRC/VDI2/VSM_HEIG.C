
#include <vdibind.h>

    WORD
vsm_height( handle, height )
WORD handle;		/* Physical device handle */
WORD height;
{
    ptsin[0] = 0;
    ptsin[1] = height;

    contrl[0] = 19;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
    return( ptsout[1] );
}

