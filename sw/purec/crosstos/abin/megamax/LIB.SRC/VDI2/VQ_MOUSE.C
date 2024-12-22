
#include <vdibind.h>

    WORD
vq_mouse( handle, status, px, py )
WORD handle;		/* Physical device handle */
WORD *status;
WORD *px;
WORD *py;
{
    contrl[0] = 124;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *status = intout[0];
    *px = ptsout[0];
    *py = ptsout[1];
}
