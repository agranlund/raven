
#include <vdibind.h>

    WORD
vrq_locator( handle, initx, inity, xout, yout, term )
WORD handle;		/* Physical device handle */
WORD initx;
WORD inity;
WORD *xout;
WORD *yout;
WORD *term;
{
    ptsin[0] = initx;
    ptsin[1] = inity;

    contrl[0] = 28;
    contrl[1] = 1;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    *xout = ptsout[0];
    *yout = ptsout[1];
    *term = intout[0];
}
