#ifndef _GEM_VDI_P_
# define _GEM_VDI_P_

# ifndef _GEMLIB_H_
#  include "mt_gem.h"
# endif


#define vdi_control_ptr(n)   *((void**)(vdi_control +n))
#define vdi_intin_ptr(n)     *((void**)(vdi_intin   +n))
#define vdi_intout_long(n)   *((long*) (vdi_intout  +n))

#if defined(__GNUC_INLINE__) && (__GNUC__ > 2 || __GNUC_MINOR__ > 5)

static __inline void
_vdi_trap_esc (VDIPB * vdipb,
               long cntrl_0_1, long nintin, long cntrl_5, _WORD handle)
{
	__asm__ volatile (
		"movea.l	%0,a0\n\t"	/* &vdipb */
		"move.l	a0,d1\n\t"
		"move.l	(a0),a0\n\t"	/* vdipb->control */
		"move.l	%1,(a0)+\n\t"	/* opcode, nptsin */
		"move.l	%2,(a0)+\n\t"	/* nptsout, nintin */
		"move.l	%3,(a0)+\n\t"	/* nintout, subop */
		"move.w	%4,(a0)\n\t"	/* handle */
		"move.w	#115,d0\n\t"	/* 0x0073 */
		"trap	#2"
		:
		: "g"(vdipb), "g"(cntrl_0_1), "g"(nintin), "g"(cntrl_5), "g"(handle)
		: "d0", "d1", "d2", "a0", "a1", "a2", "memory"
	);
}
#define VDI_TRAP_ESC(vdipb, handle, opcode, subop, nptsin, nintin) \
	_vdi_trap_esc (&vdipb, (opcode##uL<<16)|nptsin, nintin, subop, handle)

static __inline void
_vdi_trap_00 (VDIPB * vdipb, long cntrl_0_1, _WORD handle)
{
	__asm__ volatile (
		"movea.l %0,a0\n\t"	/* &vdipb */
		"move.l  a0,d1\n\t"
		"move.l  (a0),a0\n\t"	/* vdipb->control */
		"move.l  %1,(a0)+\n\t"	/* cntrl_0, nptsin */
		"moveq   #0,d0\n\t"
		"move.l  d0,(a0)+\n\t"	/* cntrl_2, nintin */
		"move.l  d0,(a0)+\n\t"	/* cntrl_4, cntrl_5 */
		"move.w  %2,(a0)\n\t"	/* handle */
		"move.w  #115,d0\n\t"	/* 0x0073 */
		"trap    #2"
		:
		: "g"(vdipb), "g"(cntrl_0_1), "g"(handle)
		: "d0", "d1", "d2", "a0", "a1", "a2", "memory"
	);
}
#define VDI_TRAP_00(vdipb, handle, opcode) \
	_vdi_trap_00 (&vdipb, (opcode##uL<<16), handle)


#else /* no usage of gnu inlines, go the old way */

#define VDI_TRAP_ESC(vdipb, handle, opcode, subop, nptsin, nintin) \
	vdi_control[0] = opcode;  \
	vdi_control[1] = nptsin; \
	vdi_control[2] = 0; \
	vdi_control[3] = nintin; \
	vdi_control[4] = 0; \
	vdi_control[5] = subop;   \
	vdi_control[6] = handle;  \
	vdi (&vdipb);

#define VDI_TRAP_00(vdipb, handle, opcode) \
	VDI_TRAP_ESC (vdipb, handle, opcode, 0, 0, 0)
#endif


#define VDI_TRAP(vdipb, handle, opcode, nptsin, nintin) \
	VDI_TRAP_ESC(vdipb, handle, opcode, 0, nptsin, nintin)

#define VDI_PARAMS(_control,_intin,_ptsin,_intout,_ptsout) \
	VDIPB vdi_params;         \
	vdi_params.contrl  = _control;   \
	vdi_params.intin   = _intin;   \
	vdi_params.ptsin   = _ptsin;   \
	vdi_params.intout  = _intout;   \
	vdi_params.ptsout  = _ptsout


#define VDI_OPCODE   vdi_control[0]
#define VDI_N_PTSIN  vdi_control[1]
#define VDI_N_PTSOUT vdi_control[2]
#define VDI_N_INTIN  vdi_control[3]
#define VDI_N_INTOUT vdi_control[4]
#define VDI_SUBCODE  vdi_control[5]
#define VDI_HANDLE   vdi_control[6]

#define N_PTRINTS 2

/* special feature for VDI bindings: pointer in parameters (for return values)
 * could be NULL (nice idea by Martin Elsasser against dummy variables) 
 */
#define CHECK_NULLPTR 1

/* special feature for VDI bindings: set VDIPB::intout and VDIPB::ptsout to
 * vdi_dummy array intead of NULL against crashes if some crazy VDI drivers
 * tries to write something in ptsout/intout.
 */ 
#define USE_VDI_DUMMY 1

#if USE_VDI_DUMMY
	/* use dummy array vdi_dummy[] from vdi_dummy.c */
	extern _WORD vdi_dummy[];
#else
	/* replace vdi_dummy in VDIPB by NULL pointer */
	#define vdi_dummy NULL
#endif

# endif /* _GEM_VDI_P_ */
