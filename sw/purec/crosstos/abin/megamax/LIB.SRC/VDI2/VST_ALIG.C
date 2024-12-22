
#include <vdibind.h>

    WORD
vst_alignment( handle, hor_in, vert_in, hor_out, vert_out )
WORD handle;		/* Physical device handle */
WORD hor_in;
WORD vert_in;
WORD *hor_out;
WORD *vert_out;
{
    intin[0] = hor_in;
    intin[1] = vert_in;

    contrl[0] = 39;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[6] = handle;
    vdi();

    *hor_out = intout[0];
    *vert_out = intout[1];
}
