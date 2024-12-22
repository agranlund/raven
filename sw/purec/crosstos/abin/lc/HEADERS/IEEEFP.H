/**
 * $Id: ieeefp.h,v 1.1 1993/11/14 14:43:52 AGK Rel $
 *
 * Lattice FPU math control
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _IEEEFP_H
#define _IEEEFP_H

long __getfpcr(void);
long __getfpsr(void);
void __setfpcr(long);
void __setfpsr(long);

extern int _FPCmode;
extern long _FPCfpcr;

typedef enum fp_rnd {
	FP_RN = 0x0000,	/* round to nearest */
	FP_RZ = 0x0010,	/* round toward zero */
	FP_RM = 0x0020,	/* round toward minus infinity */
	FP_RP = 0x0030	/* round toward minus infinity */
} fp_rnd;

fp_rnd fpgetround(void);
fp_rnd fpsetround(fp_rnd);

typedef enum fp_prec {
	FP_PX = 0x0000,	/* extended precision */
	FP_PS = 0x0040,	/* single precision */
	FP_PD = 0x0080	/* double precision */
} fp_prec;

fp_prec fpgetprecision(void);
fp_prec fpsetprecision(fp_prec);

typedef enum fp_except {
	FP_X_BSUN = 0x8000,		/* branch/set on unordered */
	FP_X_SNAN = 0x4000,		/* signalling not a number */
	FP_X_OPERR = 0x2000,	/* operand error */
#define FP_X_IOP	FP_X_OPERR	/* invalid operation (accrued) */
	FP_X_OVFL = 0x1000,		/* overflow */
	FP_X_UNFL = 0x0800,		/* underflow */
	FP_X_DZ = 0x0400,		/* divide by zero */
	FP_X_INEX2 = 0x0200,	/* inexact operation */
#define FP_X_INEX	FP_X_INEX2	/* inexact operation (accrued) */
	FP_X_INEX1 = 0x0100		/* inexact decimal input */
} fp_except;

fp_except fpgetmask(void);
fp_except fpsetmask(fp_except);


fp_except fpgetsticky(void);
fp_except fpsetsticky(fp_except);
#endif
