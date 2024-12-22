
#include <vdibind.h>

    WORD
vqf_attributes( handle, attributes )
WORD handle;		/* Physical device handle */
WORD attributes[];
{
    i_intout( attributes );

    contrl[0] = 37;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_intout( intout );
}
