
#include <vdibind.h>

    WORD
v_pline( handle, count, xy )
WORD handle;		/* Physical device handle */
WORD count;
WORD xy[];
{
    i_ptsin( xy );

    contrl[0] = 6;
    contrl[1] = count;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
