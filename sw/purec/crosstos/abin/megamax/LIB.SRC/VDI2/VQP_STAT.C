
#include <vdibind.h>

    WORD
vqp_state( handle, port, filmnum, lightness, interlace, planes, indexes )
WORD handle;		/* Physical device handle */
WORD *port;
WORD *filmnum;
WORD *lightness;
WORD *interlace;
WORD *planes;
WORD *indexes;
{
    WORD i;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 92;
    contrl[6] = handle;
    vdi();

    *port = intout[0];
    *filmnum = intout[1];
    *lightness = intout[2];
    *interlace = intout[3];
    *planes = intout[4];
    for (i = 5; i < contrl[4]; i++);
        *indexes++ = intout[i];
}
