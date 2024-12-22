
#include <vdibind.h>

    WORD
vqp_error( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 96;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
