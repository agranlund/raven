
#include <vdibind.h>

    WORD
vqt_attributes( handle, attributes )
WORD handle;		/* Physical device handle */
WORD attributes[];
{
    i_intout( attributes );
    i_ptsout( attributes+6 );

    contrl[0] = 38;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_intout( intout );
    i_ptsout( ptsout );
}
