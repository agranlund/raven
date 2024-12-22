
#include <vdibind.h>

    WORD
vrq_valuator( handle, val_in, val_out, term )
WORD handle;		/* Physical device handle */
WORD val_in;
WORD *val_out;
WORD *term;
{
    intin[0] = val_in;

    contrl[0] = 29;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();

    *val_out = intout[0];
    *term = intout[1];
}
