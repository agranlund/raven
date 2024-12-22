
#include <vdibind.h>

    WORD
v_write_meta( handle, num_ints, ints, num_pts, pts )
WORD handle;		/* Physical device handle */
WORD num_ints;
WORD ints[];
WORD num_pts;
WORD pts[];
{
    i_intin( ints );
    i_ptsin( pts );

    contrl[0] = 5;
    contrl[1] = num_pts;
    contrl[3] = num_ints;
    contrl[5] = 99;
    contrl[6] = handle;
    vdi();

    i_intin( intin );
    i_ptsin( ptsin );
}
