
#include <vdibind.h>

    WORD
vr_trnfm( handle, srcMFDB, desMFDB )
WORD handle;		/* Physical device handle */
WORD *srcMFDB;
WORD *desMFDB;
{
    i_ptr( srcMFDB );
    i_ptr2( desMFDB );

    contrl[0] = 110;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();
}
