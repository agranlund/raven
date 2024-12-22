
#include <vdibind.h>

    WORD
vq_chcells( handle, rows, columns )
WORD handle;		/* Physical device handle */
WORD *rows;
WORD *columns;
{
    contrl[0] = 5;
    contrl[1] = 0;
    contrl[3] = 0;
    contrl[5] = 1;
    contrl[6] = handle;
    vdi();

    *rows = intout[0];
    *columns = intout[1];
}
