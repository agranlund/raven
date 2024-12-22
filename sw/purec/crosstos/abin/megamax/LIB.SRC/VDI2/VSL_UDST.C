
#include <vdibind.h>

    WORD
vsl_udsty( handle, pattern )
WORD handle;		/* Physical device handle */
WORD pattern;
{
    intin[0] = pattern;

    contrl[0] = 113;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
}
