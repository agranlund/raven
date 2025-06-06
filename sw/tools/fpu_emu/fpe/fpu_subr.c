/*	$NetBSD: fpu_subr.c,v 1.12 2013/04/21 02:50:48 isaki Exp $ */

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)fpu_subr.c	8.1 (Berkeley) 6/11/93
 */

/*
 * FPU subroutines.
 */

/*
 * This file has been modified for Atari FPU emulator
 */
#include "fpe_atari.h"
/*
#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: fpu_subr.c,v 1.12 2013/04/21 02:50:48 isaki Exp $");

#include <sys/types.h>
#include <sys/systm.h>
*/
#include <machine/reg.h>

#include "fpu_emulate.h"
#include "fpu_arith.h"

#if defined(FPE_SUPPORT_68000)
int bfffo_68000(uint32_t src)
{
	int offset;
	int width = 32;
	for (offset = 0; width-- > 0 && (int)src >= 0; src <<= 1) {
		offset++;
	}
	return offset;
}
int (*bfffo)(uint32_t) = bfffo_68000;
#else
static inline int bfffo(uint32_t src)
{
	int offset;
	__asm volatile("bfffo %1{#0:#32},%0" : "=d"(offset) : "g"(src));
	int width = 32;
	for (offset = 0; width-- > 0 && (int)src >= 0; src <<= 1) {
		offset++;
	}
	return offset;
}
#endif

/*
 * Shift the given number right rsh bits.  Any bits that `fall off' will get
 * shoved into the sticky field; we return the resulting sticky.  Note that
 * shifting NaNs is legal (this will never shift all bits out); a NaN's
 * sticky field is ignored anyway.
 */
int
fpu_shr(struct fpn *fp, int rsh)
{
	uint32_t m0, m1, m2, s;
	int lsh;

#ifdef DIAGNOSTIC
	if (rsh < 0 || (fp->fp_class != FPC_NUM && !ISNAN(fp)))
		panic("fpu_rightshift 1");
#endif

	m0 = fp->fp_mant[0];
	m1 = fp->fp_mant[1];
	m2 = fp->fp_mant[2];

	/* If shifting all the bits out, take a shortcut. */
	if (rsh >= FP_NMANT) {
#ifdef DIAGNOSTIC
		if ((m0 | m1 | m2) == 0)
			panic("fpu_rightshift 2");
#endif
		fp->fp_mant[0] = 0;
		fp->fp_mant[1] = 0;
		fp->fp_mant[2] = 0;
#ifdef notdef
		if ((m0 | m1 | m2) == 0)
			fp->fp_class = FPC_ZERO;
		else
#endif
			fp->fp_sticky = 1;
		return (1);
	}

	/* Squish out full words. */
	s = fp->fp_sticky;
	if (rsh >= 32 * 2) {
		s |= m2 | m1;
		m2 = m0, m1 = 0, m0 = 0;
	} else if (rsh >= 32) {
		s |= m2;
		m2 = m1, m1 = m0, m0 = 0;
	}

	/* Handle any remaining partial word. */
	if ((rsh &= 31) != 0) {
		lsh = 32 - rsh;
		s |= m2 << lsh;
		m2 = (m2 >> rsh) | (m1 << lsh);
		m1 = (m1 >> rsh) | (m0 << lsh);
		m0 >>= rsh;
	}
	fp->fp_mant[0] = m0;
	fp->fp_mant[1] = m1;
	fp->fp_mant[2] = m2;
	fp->fp_sticky = s;
	return (s);
}

/*
 * Force a number to be normal, i.e., make its fraction have all zero
 * bits before FP_1, then FP_1, then all 1 bits.  This is used for denorms
 * and (sometimes) for intermediate results.
 *
 * Internally, this may use a `supernormal' -- a number whose fp_mant
 * is greater than or equal to 2.0 -- so as a side effect you can hand it
 * a supernormal and it will fix it (provided fp->fp_mant[2] == 0).
 */
void
fpu_norm(struct fpn *fp)
{
	uint32_t m0, m1, m2, sup, nrm;
	int lsh, rsh, exp;

	exp = fp->fp_exp;
	m0 = fp->fp_mant[0];
	m1 = fp->fp_mant[1];
	m2 = fp->fp_mant[2];

	/* Handle severe subnormals with 32-bit moves. */
	if (m0 == 0) {
		if (m1) {
			m0 = m1;
			m1 = m2;
			m2 = 0;
			exp -= 32;
		} else if (m2) {
			m0 = m2;
			m1 = 0;
			m2 = 0;
			exp -= 2 * 32;
		} else {
			fp->fp_class = FPC_ZERO;
			return;
		}
	}

	/* Now fix any supernormal or remaining subnormal. */
	nrm = FP_1;
	sup = nrm << 1;
	if (m0 >= sup) {
		/*
		 * We have a supernormal number.  We need to shift it right.
		 * We may assume m2==0.
		 */
		rsh = bfffo(m0);
		rsh = 31 - rsh - FP_LG;
		exp += rsh;
		lsh = 32 - rsh;
		m2 = m1 << lsh;
		m1 = (m1 >> rsh) | (m0 << lsh);
		m0 = (m0 >> rsh);
	} else if (m0 < nrm) {
		/*
		 * We have a regular denorm (a subnormal number), and need
		 * to shift it left.
		 */
		lsh = bfffo(m0);
		lsh = FP_LG - 31 + lsh;
		exp -= lsh;
		rsh = 32 - lsh;
		m0 = (m0 << lsh) | (m1 >> rsh);
		m1 = (m1 << lsh) | (m2 >> rsh);
		m2 <<= lsh;
	}

	fp->fp_exp = exp;
	fp->fp_mant[0] = m0;
	fp->fp_mant[1] = m1;
	fp->fp_mant[2] = m2;
}

/*
 * Concoct a `fresh' Quiet NaN per Appendix N.
 * As a side effect, we set OPERR for the current exceptions.
 */
struct fpn *
fpu_newnan(struct fpemu *fe)
{
	struct fpn *fp;

	fe->fe_fpsr |= FPSR_OPERR;
	fp = &fe->fe_f3;
	fp->fp_class = FPC_QNAN;
	fp->fp_sign = 0;
	fp->fp_mant[0] = FP_1 - 1;
	fp->fp_mant[1] = fp->fp_mant[2] = ~0;
	return (fp);
}
