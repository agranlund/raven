

/*
			VDI BINDING ROUTINES
			--------------------

	The following routines are the actual binding routines found in the 
VDIBIND library. They have been provided to help clear up any discrepancy
within the GEM Programmer's Guide Volume 1: VDI.
*/
#ifndef DL_VDIBIND
#define DL_VDIBIND


#include    "portab.h"

extern    WORD    contrl[];
extern    WORD    intin[];
extern    WORD    ptsin[];
extern    WORD    intout[];
extern    WORD    ptsout[];

extern    struct {
	WORD *pblock, *iioff, *pioff, *iooff, *pooff;
	} pblock;

extern    vdi();

#define i_ptsin(ptr) pblock.pioff = ptr
#define i_intin(ptr) pblock.iioff = ptr
#define i_intout(ptr) pblock.iooff = ptr
#define i_ptsout(ptr) pblock.pooff = ptr

extern    i_ptr();
extern    i_ptr2();
extern    m_lptr2();

#endif DL_VDIBIND
