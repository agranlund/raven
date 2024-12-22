
#include <vdibind.h>


    WORD
v_clsvwk( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 101;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
