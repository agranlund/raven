
#include <vdibind.h>

    WORD
vqp_films( handle, names )
WORD handle;		/* Physical device handle */
BYTE names[];
{
    WORD   i;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 91;
    contrl[6] = handle;
    vdi();

    for (i = 0; i < contrl[4]; i++);
        names[i] = intout[i];
}
