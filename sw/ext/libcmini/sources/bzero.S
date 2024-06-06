|	new version of bcopy and memset
|	uses movem to set 256 bytes blocks faster.
|	Alexander Lehmann	alexlehm@iti.informatik.th-darmstadt.de
|	sortof inspired by jrbs bcopy
|	has to be preprocessed (int parameter in memset)

	.text
	.even

#ifndef __FASTCALL__

#ifdef Lmemset
	.globl _memset

|	void *memset( void *dest, int val, size_t len );
|	returns dest
|	two versions for 16/32 bits

_memset:
	move.l	4(sp),a0	| dest
#ifdef __MSHORT__
	move.b	9(sp),d0	| value
# ifndef __SOZOBON__
	move.l	10(sp),d1	| length
# else
	clr.l	d1
	move.w	10(sp),d1	| length
# endif
#else
	move.b	11(sp),d0	| value
	move.l	12(sp),d1	| length
#endif
	jeq	exit			| length==0? (size_t)
#ifndef Lmemset
	jra	do_set
#endif
#endif /* Lmemset */

#ifndef Lmemset
	.globl _bzero
	.globl __bzero
	.globl ___bzero

|	void bzero( void *dest, size_t length );
|	void _bzero( void *dest, unsigned long length );
|	return value not used (returns dest)

#ifdef __SOZOBON__
___bzero:
_bzero:
	move.l	4(sp),a0	| dest
	clr.l	d1
	move.w	8(sp),d1	| length
	jra	scommon
#else
___bzero:
_bzero:
#endif
__bzero:
	move.l	4(sp),a0	| dest
	move.l	8(sp),d1	| length
scommon:
	jeq	exit		| length==0? (size_t)
	clr.b	d0		| value
#endif /* Lbzero */

#else /* __FASTCALL__ */

#ifdef Lmemset
	.globl _memset

|	void *memset( void *dest, int val, size_t len );
|	returns dest
|	two versions for 16/32 bits

_memset:
	tst.l	d1
	jeq		exit		| length==0? (size_t)
#ifndef 	Lmemset
	jra		do_set
#endif
#endif /* Lmemset */

#ifndef Lmemset
	.globl 	_bzero
	.globl 	__bzero
	.globl 	___bzero

|	void bzero( void *dest, size_t length );
|	void _bzero( void *dest, unsigned long length );
|	return value not used (returns dest)

___bzero:
_bzero:
__bzero:
	move.l	d0,d1
scommon:
	jeq		exit		| length==0? (size_t)
	clr.b	d0		| value
#endif /* Lbzero */

#endif /* __FASTCALL__ */

do_set: 			| a0 dest, d0.b byte, d1.l length
	move.l	d2,-(sp)

	add.l	d1,a0		| a0 points to end of area, needed for predec

	move.w	a0,d2		| test for alignment
	btst	#0,d2		| odd ?
	jeq		areeven
	move.b	d0,-(a0) 	| set one byte, now we are even
	subq.l	#1,d1
areeven:
	move.b	d0,d2
#ifdef __mcoldfire__
	lsl.l	#8,d0
#else
	lsl.w	#8,d0
#endif
	move.b	d2,d0
	move.w	d0,d2
	swap	d2
	move.w	d0,d2		| d2 has byte now four times

	moveq	#0,d0		| save length less 256
	move.b	d1,d0
	lsr.l	#8,d1		| number of 256 bytes blocks
	jeq		less256
#ifdef __mcoldfire__
	lea		-40(sp),sp
	movem.l	d0/d3-d7/a2/a3/a5/a6,(sp)	| d2 is already saved
#else
	movem.l	d0/d3-d7/a2/a3/a5/a6,-(sp)	| d2 is already saved
#endif
				| exclude a4 because of -mbaserel
	move.l	d2,d0
	move.l	d2,d3
	move.l	d2,d4
	move.l	d2,d5
	move.l	d2,d6
	move.l	d2,d7
	move.l	d2,a2
	move.l	d2,a3
	move.l	d2,a5
	move.l	d2,a6
set256:
#ifdef __mcoldfire__
	lea		-256(a0),a0
	movem.l	d0/d2-d7/a2/a3/a5/a6,212(a0)	| set 5*44+36=256 bytes
	movem.l	d0/d2-d7/a2/a3/a5/a6,168(a0)
	movem.l	d0/d2-d7/a2/a3/a5/a6,124(a0)
	movem.l	d0/d2-d7/a2/a3/a5/a6,80(a0)
	movem.l	d0/d2-d7/a2/a3/a5/a6,36(a0)
	movem.l	d0/d2-d7/a2-a3,(a0)
#else
	movem.l	d0/d2-d7/a2/a3/a5/a6,-(a0)	| set 5*44+36=256 bytes
	movem.l	d0/d2-d7/a2/a3/a5/a6,-(a0)
	movem.l	d0/d2-d7/a2/a3/a5/a6,-(a0)
	movem.l	d0/d2-d7/a2/a3/a5/a6,-(a0)
	movem.l	d0/d2-d7/a2/a3/a5/a6,-(a0)
	movem.l	d0/d2-d7/a2-a3,-(a0)
#endif
	subq.l	#1,d1
	jne		set256			| next, please
#ifdef __mcoldfire__
	movem.l	(sp),d0/d3-d7/a2/a3/a5/a6
	lea		40(sp),sp
#else
	movem.l	(sp)+,d0/d3-d7/a2/a3/a5/a6
#endif
less256:			| set 16 bytes blocks
	move.w	d0,-(sp) 	| save length below 256 for last 3 bytes
#ifdef __mcoldfire__
	lsr.l	#2,d0
	jeq		less4		| less that 4 bytes left
	move.l	d0,d1
	neg.l	d1
	andi.l	#3,d1		| d1 = number of bytes below 16 (-n)&3
	subq.l	#1,d0
	lsr.l	#2,d0		| number of 16 bytes blocks minus 1, if d1==0
	add.l	d1,d1		| offset in code (movl two bytes)
	jmp		2(pc,d1.l)	| jmp into loop
#else
	lsr.w	#2,d0		| number of 4 bytes blocks
	jeq		less4		| less that 4 bytes left
	move.w	d0,d1
	neg.w	d1
	andi.w	#3,d1		| d1 = number of bytes below 16 (-n)&3
	subq.w	#1,d0
	lsr.w	#2,d0		| number of 16 bytes blocks minus 1, if d1==0
	add.w	d1,d1		| offset in code (movl two bytes)
	jmp		2(pc,d1.w)	| jmp into loop
#endif
set16:
	move.l	d2,a0@-
	move.l	d2,a0@-
	move.l	d2,a0@-
	move.l	d2,a0@-
#ifdef __mcoldfire__
	subq.l	#1,d0
	bpl		set16
#else
	dbra	d0,set16
#endif
less4:
	move.w	(sp)+,d0
	btst	#1,d0
	jeq		less2
	move.w	d2,a0@-
less2:
	btst	#0,d0
	jeq		none
	move.b	d2,a0@-
none:
exit_d2:
	move.l	(sp)+,d2
exit:
	move.l a0,d0		| return dest (for memset only), in a0 by predec now
	rts
