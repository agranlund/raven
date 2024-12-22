
#include <vdibind.h>


vst_unload_fonts( handle, select )
WORD handle;		/* Physical device handle */
WORD select;
{
    contrl[0] = 120;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
