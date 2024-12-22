
#include <vdibind.h>

    WORD
v_hardcopy( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 17;
    contrl[6] = handle;
    vdi();
}
