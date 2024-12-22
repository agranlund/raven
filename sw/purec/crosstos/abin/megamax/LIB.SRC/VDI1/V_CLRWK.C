
#include <vdibind.h>


    WORD
v_clrwk( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 3;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
