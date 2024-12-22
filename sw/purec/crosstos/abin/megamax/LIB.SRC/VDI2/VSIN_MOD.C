
#include <vdibind.h>

    WORD
vsin_mode( handle, dev_type, mode )
WORD handle;		/* Physical device handle */
WORD dev_type;
WORD mode;
{
    intin[0] = dev_type;
    intin[1] = mode;

    contrl[0] = 33;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[6] = handle;
    vdi();
}
