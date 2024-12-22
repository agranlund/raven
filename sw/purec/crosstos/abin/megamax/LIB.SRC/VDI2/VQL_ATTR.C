
#include <vdibind.h>

    WORD
vql_attributes( handle, attributes )
WORD handle;		/* Physical device handle */
WORD attributes[];
{
    i_intout( attributes );

    contrl[0] = 35;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_intout( intout );

	attributes[5] = attributes[3];
	attributes[4] = attributes[3];
    attributes[3] = ptsout[0];
}
