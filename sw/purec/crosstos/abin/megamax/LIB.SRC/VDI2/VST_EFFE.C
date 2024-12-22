
#include <vdibind.h>

    WORD
vst_effects( handle, effect )
WORD handle;		/* Physical device handle */
WORD effect;
{
    intin[0] = effect;

    contrl[0] = 106;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
