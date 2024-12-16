/* libgcc1 routines for 68000 w/o floating-point hardware.
   Copyright (C) 1994, 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file with other programs, and to distribute
those programs without any restriction coming from the use of this
file.  (The General Public License restrictions do apply in other
respects; for example, they cover modification of the file, and
distribution when not linked into another program.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details. */

/* As a special exception, if you link this library with files
   compiled with GCC to produce an executable, this does not cause
   the resulting executable to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

/* Use this one for any 680x0; assumes no floating point hardware.
   The trailing " '" appearing on some lines is for ANSI preprocessors.  Yuk.
   Some of this code comes from MINIX, via the folks at ericsson.
   D. V. Henkel-Wallace (gumby@cygnus.com) Fete Bastille, 1992
*/

// These are predefined by new versions of GNU cpp.

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

#ifndef __IMMEDIATE_PREFIX__
#define __IMMEDIATE_PREFIX__ #
#endif

// ANSI concatenation macros.
#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b

// Use the right prefix for global labels.
#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

// Use the right prefix for registers.
#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

// Use the right prefix for immediate values.
#define IMM(x) CONCAT1 (__IMMEDIATE_PREFIX__, x)

#define d0 REG (d0)
#define d1 REG (d1)
#define d2 REG (d2)
#define d3 REG (d3)
#define d4 REG (d4)
#define d5 REG (d5)
#define d6 REG (d6)
#define d7 REG (d7)
#define a0 REG (a0)
#define a1 REG (a1)
#define a2 REG (a2)
#define a3 REG (a3)
#define a4 REG (a4)
#define a5 REG (a5)
#define a6 REG (a6)
#define fp REG (fp)
#define sp REG (sp)

	.text
	.globl	SYM (__mulsi3)
	.globl	SYM (__divsi3)
	.globl	SYM (__udivsi3)
	.globl	SYM (__modsi3)
	.globl	SYM (__umodsi3)
	.globl	SYM (__main)
	.globl	SYM (__atexit)

SYM (__main):
	rts

SYM (__atexit):
    rts

//-------------------------------------------------------------------
//
//	__mmulsi3
//
//-------------------------------------------------------------------
SYM (__mulsi3):
	movew	sp@(4), d0
	muluw	sp@(10), d0
	movew	sp@(6), d1
	muluw	sp@(8), d1
#if !(defined(__mcf5200__) || defined(__mcoldfire__))
	addw	d1, d0
#else
	addl	d1, d0
#endif
	swap	d0
	clrw	d0
	movew	sp@(6), d1
	muluw	sp@(10), d1
	addl	d1, d0
	rts


//-------------------------------------------------------------------
//
//	__divsi3
//
//-------------------------------------------------------------------
SYM (__divsi3):
	movel	d2, sp@-
	moveq	IMM (1), d2
	movel	sp@(12), d1
	jpl	L1a
	negl	d1
#if !(defined(__mcf5200__) || defined(__mcoldfire__))
	negb	d2
#else
	negl	d2
#endif
L1a:
	movel	sp@(8), d0
	jpl	L2a
	negl	d0
#if !(defined(__mcf5200__) || defined(__mcoldfire__))
	negb	d2
#else
	negl	d2
#endif
L2a:
	movel	d1, sp@-
	movel	d0, sp@-
	jbsr	SYM (__udivsi3)
	addql	IMM (8), sp
	tstb	d2
	jpl	L3a
	negl	d0
L3a:
	movel	sp@+, d2
	rts


//-------------------------------------------------------------------
//
//	__udivsi3
//
//-------------------------------------------------------------------
SYM (__udivsi3):
#if !(defined(__mcf5200__) || defined(__mcoldfire__))
	movel	d2, sp@-
	movel	sp@(12), d1
	movel	sp@(8), d0
	cmpl	IMM (0x10000), d1
	jcc	L3
	movel	d0, d2
	clrw	d2
	swap	d2
	divu	d1, d2
	movew	d2, d0
	swap	d0
	movew	sp@(10), d2
	divu	d1, d2
	movew	d2, d0
	jra	L6
L3:	movel	d1, d2
L4:	lsrl	IMM (1), d1
	lsrl	IMM (1), d0
	cmpl	IMM (0x10000), d1
	jcc	L4
	divu	d1, d0
	andl	IMM (0xffff), d0
	// Multiply the 16 bit tentative quotient with the 32 bit divisor.  Because of
	// the operand ranges, this might give a 33 bit product.  If this product is
	// greater than the dividend, the tentative quotient was too large.
	movel	d2, d1
	mulu	d0, d1
	swap	d2
	mulu	d0, d2
	swap	d2
	tstw	d2
	jne	L5
	addl	d2, d1
	jcs	L5
	cmpl	sp@(8), d1
	jls	L6
L5:	subql	IMM (1), d0
L6:	movel	sp@+, d2
	rts
#else // __mcf5200__ || __mcoldfire__
	// Coldfire implementation of non-restoring division algorithm from
	//   Hennessy & Patterson, Appendix A.
	link	a6,IMM (-12)
	moveml	d2-d4,sp@
	movel	a6@(8),d0
	movel	a6@(12),d1
	clrl	d2
	moveq	IMM (31),d4
L1:	addl	d0,d0
	addxl	d2,d2
	movl	d2,d3
	subl	d1,d3
	jcs	L2
	bset	IMM (0),d0
	movl	d3,d2
L2:	subql	IMM (1),d4
	jcc	L1
	moveml	sp@,d2-d4
	unlk	a6
	rts
#endif // __mcf5200__ || __mcoldfire__


//-------------------------------------------------------------------
//
//	__modsi3
//
//-------------------------------------------------------------------
SYM (__modsi3):
	movel	sp@(8), d1
	movel	sp@(4), d0
	movel	d1, sp@-
	movel	d0, sp@-
	jbsr	SYM (__divsi3)
	addql	IMM (8), sp
	movel	sp@(8), d1
#if !(defined(__mcf5200__) || defined(__mcoldfire__))
	movel	d1, sp@-
	movel	d0, sp@-
	jbsr	SYM (__mulsi3)
	addql	IMM (8), sp
#else
	mulsl	d1,d0
#endif
	movel	sp@(4), d1
	subl	d0, d1
	movel	d1, d0
	rts

//-------------------------------------------------------------------
//
//	__umodsi3
//
//-------------------------------------------------------------------
SYM (__umodsi3):
	movel	sp@(8), d1
	movel	sp@(4), d0
	movel	d1, sp@-
	movel	d0, sp@-
	jbsr	SYM (__udivsi3)
	addql	IMM (8), sp
	movel	sp@(8), d1
#if !(defined(__mcf5200__) || defined(__mcoldfire__))
	movel	d1, sp@-
	movel	d0, sp@-
	jbsr	SYM (__mulsi3)
	addql	IMM (8), sp
#else
	mulsl	d1,d0
#endif
	movel	sp@(4), d1
	subl	d0, d1
	movel	d1, d0
	rts
