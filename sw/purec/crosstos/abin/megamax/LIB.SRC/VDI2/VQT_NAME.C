
#include <vdibind.h>

    WORD
vqt_name( handle, element_num, name )
WORD handle;		/* Physical device handle */
WORD element_num;
BYTE name[];
{
    WORD i;

    intin[0] = element_num;

    contrl[0] = 130;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    for (i = 0 ; i < 32 ; i++)
	name[i] = intout[i + 1];
    return( intout[0] );
}
