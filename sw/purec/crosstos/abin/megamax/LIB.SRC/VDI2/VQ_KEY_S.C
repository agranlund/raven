
#include <vdibind.h>

    WORD
vq_key_s( handle, status )
WORD handle;		/* Physical device handle */
WORD *status;
{
    contrl[0] = 128;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *status = intout[0];
}
