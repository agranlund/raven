
#include <vdibind.h>

    WORD
vm_filename( handle, filename )
WORD handle;		/* Physical device handle */
BYTE *filename;
{
    WORD *intstr;

    intstr = intin;
    while( *intstr++ = *filename++ )
        ;

    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = ((int)(intstr - intin)-1);
    contrl[5] = 100;
    contrl[6] = handle;
    vdi();
}
