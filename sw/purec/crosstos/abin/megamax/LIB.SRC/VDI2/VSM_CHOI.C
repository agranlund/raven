
#include <vdibind.h>

    WORD
vsm_choice( handle, choice )
WORD handle;		/* Physical device handle */
WORD *choice;
{
    contrl[0] = 30;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *choice = intout[0];
    return( contrl[4] );
}
