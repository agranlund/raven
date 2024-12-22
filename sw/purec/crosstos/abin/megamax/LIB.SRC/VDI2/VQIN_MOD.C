
#include <vdibind.h>

    WORD
vqin_mode( handle, dev_type, mode )
WORD handle;		/* Physical device handle */
WORD dev_type;
WORD *mode;
{
    intin[0] = dev_type;

    contrl[0] = 115;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    *mode = intout[0];
}
