
#include <vdibind.h>

v_curhome ( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 8;
    contrl[6] = handle;
    vdi();
}
