
#include <vdibind.h>

    WORD
vst_load_fonts( handle, select )
WORD handle;		/* Physical device handle */
WORD select;
{
    contrl[0] = 119;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
