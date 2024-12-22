/*
			AES BINDING ROUTINES
			--------------------

	The following routines are the actual binding routines found in the 
AESBIND library. They have been provided to help clear up any discrepancy
within the GEM Programmer's Guide Volume 2: AES.
*/

#include <portab.h>
#include <gembind.h>

extern crystal(); 	/*	Used by crys_if to do the actual AES trap call. */
extern ctrl_cnts();	/* actually a table of numbers */

int control[C_SIZE+1], global[G_SIZE+1]; 
int_in[I_SIZE+1], int_out[O_SIZE+1];
long addr_in[AI_SIZE+1], addr_out[AO_SIZE+1];

/*
	Changed structure to non-static global.
	Changed name from 'c' to '_c'.
	Changed name from 'ad_c' to '_ad_c'.

	rpt 8-21-87
*/
struct __c _c, *_ad_c;

int gl_apid;	/* application ID */


crys_if(opcode)

	WORD		opcode;		/* The opcode for the AES function */
{
	WORD		i;		/* temp variable		   */
	BYTE		*pctrl;		/* Pointer to the applications     */
					/* control arrays		   */
	control[0] = opcode;

	pctrl = &(((char *)(ctrl_cnts))[(opcode - 10) * 3]);
	for(i=1; i<C_SIZE; i++)
	  control[i] = *pctrl++;

	crystal(_ad_c);
	return( RET_CODE );
}
