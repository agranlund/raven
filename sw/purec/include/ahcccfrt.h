/* Copyright (c) 2006 by H. Robbers.
 *
 * This file is part of AHCC.
 *
 * AHCC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * AHCC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AHCC; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* This file is invoked by AHCC before any byte is compiled.
   Its purpose is a flexible way of handling all kinds of AHCC
   specific, mostly non standard, stuff.
   The file is read automatically without the need for a #include
 */

/* Aint & Auint are defined such that you will always get 2-byte
   ints independent of __INT4__ setting.
   It is mainly intended for use with AHCCLIB header files.
   AHCCLIB is a 16-bit int library, but you can have a 32-bit
   int application.
*/

#ifndef AHCC_RUN_H
#define AHCC_RUN_H

#if __ABC__ || __AHCC__

	#if defined(__68020__) || defined(__COLDFIRE__)
		#if defined(__LONGLONG__) && __LONGLONG__
		/* long long stuff; routines in ahcclib\ll.s */

			#define __ll long long

			__ll __OP__ + (__ll, __ll) _lladd;
			__ll __OP__ - (__ll, __ll) _llsub;
			__ll __OP__ * (__ll, __ll) _llmul;
			__ll __OP__ / (__ll, __ll) _lldiv;
			__ll __OP__ % (__ll, __ll) _llmod;
			__ll __OP__ & (__ll, __ll) _lland;
			__ll __OP__ | (__ll, __ll) _llor;
			__ll __OP__ ^ (__ll, __ll) _lleor;
			__ll __OP__ << (__ll, __ll) _llshl;
			__ll __OP__ >> (__ll, __ll) _llshr;

			_Bool __OP__ == (__ll, __ll) _lleq;
			_Bool __OP__ != (__ll, __ll) _llne;
			_Bool __OP__ <  (__ll, __ll) _lllt;
			_Bool __OP__ >  (__ll, __ll) _llgt;
			_Bool __OP__ >= (__ll, __ll) _llge;
			_Bool __OP__ <= (__ll, __ll) _llle;

			__ll __OP__ - (__ll) _llneg;

			__ll __UC__ (char)			_b2ll;
			__ll __UC__ (unsigned char) _ub2ll;
			__ll __UC__ (short)			_s2ll;
			__ll __UC__ (unsigned short) _us2ll;
			__ll __UC__ (long)			_l2ll;
			__ll __UC__ (unsigned long) _ul2ll;

			__ll __UC__ (float)		_f2ll;
			float __UC__ (__ll)		_ll2f;
			__ll __UC__ (double)	_d2ll;
			double __UC__(__ll) 	_ll2d;

			#undef __ll
		#endif
	#else
/*		#message long multiply, mod and divide handled by software	*/
		#define __HAVE_SW_LONG_MUL_DIV__ 1
		/* The operands are casted before the existence of these operator
		   overloads are examined, so the below will suffice. */
		unsigned long __OP__ / (unsigned long, unsigned long) _uldiv;
		         long __OP__ / (         long,          long) _ldiv;
		unsigned long __OP__ * (unsigned long, unsigned long) _ulmul;
		         long __OP__ * (         long,          long) _lmul;
		unsigned long __OP__ % (unsigned long, unsigned long) _ulmod;
		         long __OP__ % (         long,          long) _lmod;
	#endif
#endif

#endif
