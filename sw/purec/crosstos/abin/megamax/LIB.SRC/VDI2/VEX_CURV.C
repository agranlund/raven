
#include <vdibind.h>

    WORD
vex_curv( handle, usercode, savecode )
WORD handle;		/* Physical device handle */
LONG usercode;
LONG *savecode;
{
    i_ptr( usercode );

    contrl[0] = 127;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    m_lptr2( savecode );
}
