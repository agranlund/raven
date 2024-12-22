
#include <vdibind.h>

    WORD
vsm_type( handle, symbol )
WORD handle;		/* Physical device handle */
WORD symbol;
{
    intin[0] = symbol;

    contrl[0] = 18;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
