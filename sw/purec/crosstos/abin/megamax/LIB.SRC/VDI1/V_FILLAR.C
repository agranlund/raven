
#include <vdibind.h>

    WORD
v_fillarea( handle, count, xy)
WORD handle;		/* Physical device handle */
WORD count;
WORD xy[];
{
    i_ptsin( xy );

    contrl[0] = 9;
    contrl[1] = count;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
