
#include <vdibind.h>

    WORD
v_clear_disp_list( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 22;
    contrl[6] = handle;
    vdi();
}
