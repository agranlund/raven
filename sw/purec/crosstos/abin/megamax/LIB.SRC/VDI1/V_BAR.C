
#include <vdibind.h>

    WORD
v_bar( handle, xy )
WORD handle;		/* Physical device handle */
WORD xy[];
{
    i_ptsin( xy );

    contrl[0] = 11;
    contrl[1] = 2;
    contrl[3] = 0;
    contrl[5] = 1;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
