
#include <vdibind.h>

    WORD
v_updwk( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 4;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
