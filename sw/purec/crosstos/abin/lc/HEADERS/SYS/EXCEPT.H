/**
 * $Id: except.h,v 1.1 1993/11/14 14:58:56 AGK Rel $
 *
 * M680x0 family exception frames
 *
 * (c) Copyright 1991, 1993 HiSoft
**/

#ifndef _SYS_EXCEPT_H
#define _SYS_EXCEPT_H

typedef struct except {
	union {
		struct {
			union {
				struct {
					short sr;	/* status register */
					void *pc;	/* program counter */
				} g0;			/* group 0 */
				struct {
					short sr;	/* status register */
					void *pc;	/* program counter */
				} g1;			/* group 1 */
				struct {
					short ssr;	/* special sr */
					void *aa;	/* access address */
					short ir;	/* instruction register */
					short sr;	/* status register */
					void *pc;	/* program counter */
				} g2;			/* group 2 */
			} g;
		} mc68000;
		struct {
			short sr;			/* status register */
			void *pc;			/* program counter */
			unsigned short fv;	/* format & vector offset */
			union {
				/* format 0 */
				/* format 1 */
				struct {
					void *ia;	/* instruction address */
				} f2;				/* format 2 */
				struct {
					short i[29];
				} f7;				/* format 7 */
				struct {
					short ssr;	/* special sr */
					void *dcfa;	/* data cycle fault address */
					short i1;	/* internal */
					void *dob;	/* data output buffer */
					short i2;	/* internal */
					void *dib;	/* data input buffer */
					short i3;	/* internal */
					void *iib;	/* input input buffer */
					short i4;	/* internal */
					short i5;	/* internal */
					short i6;	/* internal */
					short i7;	/* internal */
					short i8;	/* internal */
					short i9;	/* internal */
					short i10;	/* internal */
					short i11;	/* internal */
					short i12;	/* internal */
					short i13;	/* internal */
					short i14;	/* internal */
					short i15;	/* internal */
					short i16;	/* internal */
					short i17;	/* internal */
					short i18;	/* internal */
					short i19;	/* internal */
				} f8;				/* format 8 */
				struct {
					void *ia;	/* instruction address */
					short i1;	/* internal */
					short i2;	/* internal */
					short i3;	/* internal */
					short i4;	/* internal */
				} f9;				/* format 9 */
				struct {
					short i1;	/* internal */
					short ssr;	/* special sr */
					short ipc;	/* instruction pipe stage C */
					short ipb;	/* instruction pipe stage B */
					void *dcfa;	/* data cycle fault address */
					short i2;	/* internal */
					short i3;	/* internal */
					void *dob;	/* data output buffer */
					short i4;	/* internal */
					short i5;	/* internal */
				} f10;			/* format 10 */
				struct {
					short i1;	/* internal */
					short ssr;	/* special sr */
					short ipc;	/* instruction pipe stage C */
					short ipb;	/* instruction pipe stage B */
					void *dcfa;	/* data cycle fault address */
					short i2;	/* internal */
					short i3;	/* internal */
					void *dob;	/* data output buffer */
					short i4;	/* internal */
					short i5;	/* internal */
					short i6;	/* internal */
					short i7;	/* internal */
					void *sba;	/* stage B address */
					short i8;	/* internal */
					short i9;	/* internal */
					void *dib;	/* data input buffer */
					short i10;	/* internal */
					short i11;	/* internal */
					short i12;	/* internal */
					short vni;	/* version no. & internal */
					short i13;	/* internal */
					short i14;	/* internal */
					short i15;	/* internal */
					short i16;	/* internal */
					short i17;	/* internal */
					short i18;	/* internal */
					short i19;	/* internal */
					short i20;	/* internal */
					short i21;	/* internal */
					short i22;	/* internal */
					short i23;	/* internal */
					short i24;	/* internal */
					short i25;	/* internal */
					short i26;	/* internal */
					short i27;	/* internal */
					short i28;	/* internal */
					short i29;	/* internal */
					short i30;	/* internal */
				} f11;			/* format 11 */
				struct {
					short i;	/* format unknown */
				} f12;			/* format 12 */
			} f;
		} mc680x0;
	} m;
} except_t;
#endif
