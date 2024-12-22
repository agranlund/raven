
#include <vdibind.h>

    WORD
v_opnwk( work_in, handle, work_out )
WORD work_in[];
WORD *handle;		/* Physical device handle */
WORD work_out[];	/* Output arguments array */
{
   i_intin( work_in );
   i_intout( work_out );
   i_ptsout( work_out + 45 );

   contrl[0] = 1;
   contrl[1] = 0;        /* no points to xform */
   contrl[3] = 11;        /* pass down xform mode also */
   vdi();

   *handle = contrl[6];    

   i_intin( intin );
   i_intout( intout );
   i_ptsout( ptsout );
   i_ptsin( ptsin );	/* must set in 68k land so we can ROM it */
}
