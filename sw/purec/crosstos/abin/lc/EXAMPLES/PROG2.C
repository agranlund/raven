#include <aes.h>		/* get the AES prototypes and definitions */
#include <vdi.h>		/* get the VDI prototypes and definitions */

/* v_opnvwk input array */
short work_in[11] = {1,1,1,1,1,1,1,1,1,1,2};

/* v_opnvwk output array */
short work_out[57];

int
main(void)
{
	short handle;		/* virtual workstation handle */
	short junk;			/* unused variable */
	
	appl_init();		/* start AES */
	handle=graf_handle(&junk, &junk, &junk, &junk);	/* find AES handle */
	v_opnvwk(work_in, handle, work_out);			/* open workstation */
	v_clrwk(handle);								/* clear workstation */

	vsf_interior(handle; FIS_USER);	/* select fill type user-defined */
									/* draw a circle on screen */
	v_circle(handle, work_out[0]/2, work_out[1]/2, work_out[1]/2);

	v_clsvwk(handle);				/* close workstation */
	evnt_keybd();
	appl_exit();					/* shutdown AES */
	return 0;
}
