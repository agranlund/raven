
#include <vdibind.h>

    WORD
vsp_save( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 94;
    contrl[6] = handle;
    vdi();
}
