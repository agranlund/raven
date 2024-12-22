
#include <vdibind.h>

    WORD
v_gtext( handle, x, y, string)
WORD handle;		/* Physical device handle */
WORD x;
WORD y;
unsigned char *string;
{
    WORD i;

    ptsin[0] = x;
    ptsin[1] = y;
    i = 0;
    while (intin[i++] = *string++)
        ;

    contrl[0] = 8;
    contrl[1] = 1;
    contrl[3] = --i;
    contrl[6] = handle;
    vdi();
}
