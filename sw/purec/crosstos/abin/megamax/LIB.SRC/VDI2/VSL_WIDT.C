
#include <vdibind.h>

    WORD
vsl_width( handle, width )
WORD handle;		/* Physical device handle */
WORD width;
{
    ptsin[0] = width;
    ptsin[1] = 0;

    contrl[0] = 16;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
    return( ptsout[0] );
}
