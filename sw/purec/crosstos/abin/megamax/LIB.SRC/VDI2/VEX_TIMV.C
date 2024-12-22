
#include <vdibind.h>

    WORD
vex_timv( handle, tim_addr, old_addr, scale )
WORD handle;		/* Physical device handle */
WORD *scale;
LONG tim_addr;
LONG *old_addr;
{
    i_ptr( tim_addr );

    contrl[0] = 118;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    m_lptr2( old_addr );
    *scale = intout[0];
}

