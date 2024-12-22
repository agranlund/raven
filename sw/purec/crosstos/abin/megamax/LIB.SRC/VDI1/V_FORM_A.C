
#include <vdibind.h>

    WORD
v_form_adv( handle )
WORD handle;		/* Physical device handle */
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 20;
    contrl[6] = handle;
    vdi();
}
