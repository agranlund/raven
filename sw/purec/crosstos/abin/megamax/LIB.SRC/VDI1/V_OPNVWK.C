
#include <vdibind.h>

/*
	Change for Megamax C.  _vhandle set by v_opvwk()
*/
int _vhandle = -1;

    WORD
v_opnvwk( work_in, handle, work_out )
WORD work_in[];		/* Input arguments array */
WORD *handle;		/* Physical device handle */
WORD work_out[];	/* Output arguments array */
{
   i_intin( work_in );
   i_intout( work_out );
   i_ptsout( work_out + 45 );

   contrl[0] = 100;
   contrl[1] = 0;
   contrl[3] = 11;
   contrl[6] = *handle;
   vdi();

   _vhandle = *handle = contrl[6];    

   i_intin( intin );
   i_intout( intout );
   i_ptsout( ptsout );
   i_ptsin( ptsin );	/* must set in 68k land so we can ROM it */
}
