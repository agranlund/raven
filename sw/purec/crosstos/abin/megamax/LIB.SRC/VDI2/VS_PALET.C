
#include <vdibind.h>

    WORD
vs_palette( handle, palette )
WORD handle;		/* Physical device handle */
WORD palette;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[5] = 60;
    contrl[6] = handle;
    intin[0] = palette;
    vdi();
    return( intout[0] );
}
