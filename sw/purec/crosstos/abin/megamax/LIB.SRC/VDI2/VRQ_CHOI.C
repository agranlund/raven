
#include <vdibind.h>

    WORD
vrq_choice( handle, in_choice, out_choice )
WORD handle;		/* Physical device handle */
WORD in_choice;
WORD *out_choice;
{
    intin[0] = in_choice;

    contrl[0] = 30;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    *out_choice = intout[0];
}
