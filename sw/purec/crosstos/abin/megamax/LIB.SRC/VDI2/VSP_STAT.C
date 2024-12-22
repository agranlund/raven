
#include <vdibind.h>

    WORD
vsp_state( handle, port, filmnum, lightness, interlace, planes, indexes )
WORD handle;		/* Physical device handle */
WORD port;
WORD filmnum;
WORD lightness;
WORD interlace;
WORD planes;
WORD *indexes;
{
    WORD i;

    intin[0] = port; 
    intin[1] = filmnum; 
    intin[2] = lightness;
    intin[3] = interlace;
    intin[4] = planes;
    for (i = 5; i < 20; i++);
        intin[i] = *indexes++;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 20;
    contrl[5] = 93;
    contrl[6] = handle;
    vdi();
}
