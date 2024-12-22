**
** c.s - Lattice C 5 startup routine
**
** Started 19/8/89 Alex G. Kiernan
**
** $Log: c.s,v $
** Revision 1.9  1993/11/05  11:05:34  AGK
** Auto init & FPU changes
**
** Revision 1.8  1992/06/11  17:42:58  AGK
** Removed various NOBASERs
**
** Revision 1.7  1992/01/08  10:16:58  AGK
** Merged stack checking code into startup
**
** Revision 1.6  1991/11/25  17:24:18  AGK
** Removed __AESpb initialisation; we perform this via relocation now
**
** Revision 1.5  1991/11/25  13:57:54  AGK
** Removed redundant __VDIinit() call
**
** Revision 1.4  1991/11/19  14:50:00  AGK
** Moved _LinkerDB load position for Desk Accessories
**
** Revision 1.3  1991/11/11  14:33:12  AGK
** Upgraded DA startup to use default common block stack unless overridden by
** user-definition
**
** Revision 1.2  1991/10/27  17:03:04  AGK
** Replaced NOALVS symbol with NOPCREL symbol
**
** Revision 1.1  1991/06/17  17:49:00  AGK
** Initial revision
**
** Copyright (c) 1990 HiSoft
**
	IFND	AUTO
AUTO	equ	0		Auto-detect program type
	ENDC

	IFND	CPX
CPX	equ	0		CPX-style startup
	ENDC
	
	IFND	DA
DA	EQU	0		Desk Accessory
	ENDC
	
	IFND	GST
GST	equ	0		GST Format mods
	ENDC
	
	IFND	NOPCREL
NOPCREL	equ	0		Use no external PC-relative code
	ENDC
	
	IFND	NOBASER
NOBASER	equ	0		Use no base-relative code
	ENDC
	
	IFND	REGARGS
REGARGS	equ	0		Use register calling conventions
	ENDC
	
	IFND	RESIDENT
RESIDENT	equ	0	Allow program to be resident
	ENDC
	
	IFND	SHORTINT
SHORTINT	equ	0	Use default short integers
	ENDC
	
	include	basepage.i
	include	gemdos.i
	include	sysvar.i

	xdef	___start
	
	xref	__STACK,__STKDELTA,__BLKSIZ
	xref	@_setargv,__MSTEP,__MemHeap
	xref	__CXSTI,__CXSTD,__FPCinit
	xref	__ENEED,__MNEED
	
	IFNE	RESIDENT
	xref	__DATALEN,__BSSLEN,__DATABAS,__RESBASE
	ENDC	

	IFNE	DA
	comm	__STACK_HEAP,4096
	ENDC	

	IFEQ	CPX
	xdef	__pbase,__PBASE,__base
	ENDC
	xdef	__tos,__clkstart
	xdef	__oserr,__OSERR,__country
	xdef	__saveD2A2,__savePC
	xdef	__msiz,__mblk,__maxblk

	IFNE	AUTO
	xdef	__XMODE
	
	xref	__AESpb,__AESintout
	IFNE	REGARGS
	xref	@appl_exit
	ELSE
	xref	_appl_exit
	ENDC
	ENDC
	
	xdef	_environ
	IFEQ	DA!CPX
	xdef	__ENVC,__onbrk
	IFNE	REGARGS
	xref	@_main
	ELSE
	xref	__main
	ENDC
	ELSE
	IFNE	DA
	xref	__DAHeap
	IFNE	REGARGS
	xref	@main
	ELSE
	xref	_main
	ENDC
	ELSE
	xref	_cpx_init
	ENDC
	ENDC
	
	IFNE	REGARGS
	xdef	@_XCEXIT
	ELSE
	xdef	__XCEXIT
	ENDC

	CSECT	text,0

	IFNE	GST
	xdef	__LinkerDB
	ELSE
	xref	__LinkerDB
	ENDC

___start
	IFNE	AUTO
	moveq	#2,d4
	move.l	a0,a2
	move.l	a0,d0
	bne.s	isada
	ENDC

	IFEQ	CPX
	IFNE	DA
	lea	__LinkerDB,a4
	move.l	#__STACK_HEAP,a7
	IFNE	NOBASER
	add.l	__STACK,a7
	ELSE
	add.l	__STACK(a4),a7
	ENDC
	move.l	a0,a2
	ELSE
	move.l	4(a7),a2		get program base page
	ENDC
	ENDC
	
	IFNE	RESIDENT		we're resident so we've some work to do

;	moveq	#4,d4			assume we are resident
	move.l	a2,a1
	lea	$100(a1),a1
	lea	__DATABAS,a0
	lea	___start(pc),a3
	cmp.l	a3,a1
	bne.s	gotram

;	moveq	#0,d4			until we know we're not resident
	
	add.l	p_tlen(a2),a1		add size of text
	add.l	p_dlen(a2),a1		add size of data
	
gotram	move.l	a1,a3
	
	move.l	#__DATALEN,d0		copy the static data
	bra.s	dfirst
	
dloop	move.l	(a0)+,(a1)+
dfirst	dbf	d0,dloop

zerbss	move.l	#__BSSLEN,d0		zero the bss
	moveq	#0,d1
	bra.s	bfirst

bloop	move.l	d1,(a1)+
bfirst	dbf	d0,bloop

	move.l	(a0)+,d0		fetch the relocation word
	bmi.s	allind
	move.l	a3,d3
	add.l	d0,a3

	moveq	#127,d2			and step through a GEMDOS relocation table
	add.w	d2,d2
	
addnxt	add.l	d3,(a3)			relocate a longword

nextb	move.b	(a0)+,d1		get relocation offset
	beq.s	allind
	cmp.b	#1,d1			check for jumps
	bne.s	isnorm
	add.w	d2,a3
	bra.s	nextb
isnorm	add.w	d1,a3
	bra.s	addnxt
allind
	move.l	d3,a4
	add.l	#__RESBASE,a4
	move.l	a1,a3

	ELSE

	IFEQ	DA
	IFNE	CPX
	move.l	a4,-(a7)
	ENDC
	lea	__LinkerDB,a4
	ENDC
	
	IFNE	AUTO
	moveq	#0,d4
	bra.s	notada
isada	
	lea	__LinkerDB,a4
	
	move.l	#isada2,a7

	IFEQ	CPX	CPXs don't know where their stacks are
; set base of stack for auto-detector so we overwrite the startup code
	IFNE	NOBASER
	move.l	#___start,__base
	ELSE
	move.l	#___start,__base(a4)
	ENDC
	ENDC
	
notada
	ENDC

	IFNE	DA
	IFNE	NOBASER
	move.l	#__STACK_HEAP,__base
	ELSE
	move.l	#__STACK_HEAP,__base(a4)
	ENDC
	ENDC

	ENDC

	IFEQ	CPX
	IFNE	NOBASER
	move.l	a2,__PBASE		put it somewhere so user can get to it
	ELSE
	move.l	a2,__PBASE(a4)
	ENDC
	ENDC

	IFNE	CPX
	move.l	a2,-(a7)
	ENDC
	
	pea	superstart(pc)
	move.w	#$26,-(a7)
	trap	#14
	addq.w	#6,a7
	
	IFNE	CPX
	move.l	(a7)+,a2
	ENDC

	IFEQ	DA!CPX
	IFNE	AUTO
	cmp.b	#2,d4
	beq	isada2
	ENDC
	IFEQ	RESIDENT
	move.l	p_bbase(a2),a1		fetch bss base
	add.l	p_blen(a2),a1		add size of bss
	ELSE
	move.l	a3,a1
	ENDC
	
	IFNE	NOBASER
	move.l	a1,_environ		give user environ variable
	ELSE
	move.l	a1,_environ(a4)
	ENDC
	
	lea.l	p_cmdlin(a2),a0
	cmp.b	#$7f,(a0)		Atari extended command line flag
	seq	d7
	
	move.l	p_env(a2),a5
	tst.b	(a5)			;;;;;
	bne.s	chk_env			;;;;;
	tst.b	1(a5)
	beq	envread
	addq.w	#1,a5
chk_env
	tst.b	(a5)
	beq	envread
	move.l	a5,(a1)+

	IFNE	SHORTINT
	IFNE	NOBASER
	addq.w	#1,__ENVC
	ELSE
	addq.w	#1,__ENVC(a4)
	ENDC	
	ELSE
	IFNE	NOBASER
	addq.l	#1,__ENVC
	ELSE
	addq.l	#1,__ENVC(a4)
	ENDC	
	ENDC
	
;	tst.b	d7
;	beq	nt_argv
	cmp.b	#'A',(a5)		try for a match with ARGV=
	bne.s	nt_argv
	move.l	a5,a3
	addq.w	#1,a5
	cmp.b	#'R',(a5)+
	bne	nt_pbp
	cmp.b	#'G',(a5)+
	bne	nt_pbp
	cmp.b	#'V',(a5)+
	bne	nt_pbp
	cmp.b	#'=',(a5)+
	bne	nt_pbp
	
	moveq	#0,d0
	move.b	d0,(a3)			tie off old argv
	move.l	d0,-4(a1)
	
	tst.b	d7
	beq	envread

	jsr	fixenv(pc)
	
	move.l	a1,a3
	
;	lea	1(a0),a6
;	bsr	chkstk

	move.l	a1,d0
	addq.w	#8,d0
skp_arg	tst.b	(a5)+
	bne.s	skp_arg
	move.l	a5,(a1)+
	cmp.l	a1,d0			first time through ?
	bne.s	nfst
	move.l	a5,a6
	bsr	chkstk
	move.l	a6,a5
	move.l	a5,-4(a1)
nfst	tst.b	(a5)
	bne.s	skp_arg
	clr.l	-4(a1)			NULL terminate argv[] like UN*X
	bra	go_main
	
nt_argv	cmp.b	#'_',(a5)+		try for a match with _PBP=
	bne.s	nt_pbp			This isn't Atari supported either
	cmp.b	#'P',(a5)+		but is used by Craft
	bne.s	nt_pbp
	cmp.b	#'B',(a5)+
	bne.s	nt_pbp
	cmp.b	#'P',(a5)+
	bne.s	nt_pbp
	cmp.b	#'=',(a5)+
	bne.s	nt_pbp
	subq.w	#4,a1
	tst.b	d7
	bne.s	nt_pbp
	
	moveq.l	#0,d1			_PBP found now check it
	moveq.l	#0,d0
get_pbp	move.b	(a5)+,d0
	beq.s	got_pbp
	lsl.l	#4,d1
	cmp.b	#'0',d0
	bcs.s	nt_pbp
	cmp.b	#'9',d0
	bcc.s	try_af
	and.b	#$f,d0
	add.l	d0,d1
	bra.s	get_pbp
try_af	cmp.b	#'A',d0
	bcs.s	nt_pbp
	cmp.b	#'z',d0
	bcc.s	nt_pbp
	cmp.b	#'F',d0
	bls.s	is_af
	cmp.b	#'a',d0
	bcs.s	nt_pbp

is_af	and.b	#$f,d0
	add.b	#9,d0
	add.l	d0,d1
	bra.s	get_pbp
got_pbp	cmp.l	p_parent(a2),d1
	seq	d7
	bra	chk_env

nt_pbp	tst.b	(a5)+
	bne.s	nt_pbp
	beq	chk_env
	
envread	clr.l	(a1)+
	jsr	fixenv(pc)
	move.l	a1,a3			no ARGV= found so do it the old way
	lea	p_cmdlin(a2),a0
	moveq	#0,d0
	move.b	(a0)+,d0
	cmp.b	#125,d0			Pexec always copies 125 chars
	bhi.s	no_null
	clr.b	0(a0,d0.w)		if < 125 we must null term

no_null	move.l	a0,a6
	bsr	chkstk
	
	IFNE	NOPCREL
	jsr	@_setargv
	ELSE
	jsr	@_setargv(pc)
	ENDC
	move.l	d0,a1
	
go_main	move.l	a1,d2		save a1 for argc calculation
	
	moveq	#16,d0
	IFNE	NOBASER
	cmp.w	#$104,__tos	tos 1.4 or greater ?
	ELSE
	cmp.w	#$104,__tos(a4)	tos 1.4 or greater ?
	ENDC
	bcs.s	ot2
	moveq	#64,d0

ot2
	IFNE	NOBASER
	IFNE	SHORTINT
	move.w	d0,__maxblk
	ELSE
	move.l	d0,__maxblk
	ENDC
	ELSE
	IFNE	SHORTINT
	move.w	d0,__maxblk(a4)
	ELSE
	move.l	d0,__maxblk(a4)
	ENDC
	ENDC

	IFNE	NOBASER
	move.l	a1,__mblk
	ELSE
	move.l	a1,__mblk(a4)
	ENDC

	move.l	d0,d1
	bra.s	fst
cmb	clr.l	(a1)+
fst	dbf	d1,cmb

	IFNE	NOBASER
	move.l	a1,__msiz
	ELSE
	move.l	a1,__msiz(a4)
	ENDC

	bra.s	fmst
cms	clr.l	(a1)+
fmst	dbf	d0,cms

	IFNE	NOBASER
	move.l	__MNEED,d0	find initial heap size
	ELSE
	move.l	__MNEED(a4),d0	find initial heap size
	ENDC
	cmp.l	#512,d0		minimum size
	ble.s	nomneed
	addq.l	#3,d0
	and.w	#~3,d0
	cmp.l	#$1fffc,d0
	bge.s	nomneed
	
	IFNE	NOBASER
	move.l	a1,__MemHeap	setup heap pointer
	ELSE
	move.l	a1,__MemHeap(a4)	setup heap pointer
	ENDC

	clr.l	(a1)+		no next block
	move.l	d0,(a1)+	length available
	subq.l	#4,d0
	move.l	d0,(a1)+	length free
	addq.w	#4,a1
	move.l	a1,-4(a1)	rover
	move.w	#$8000,(a1)+
	clr.l	(a1)+
	move.l	d0,d1
	lsr.l	#2,d0
	bset	#15,d0
	move.w	d0,(a1)
	add.l	d1,a1

nomneed
	IFNE	NOBASER
	add.l	__STKDELTA,a1	add on chicken factor
	ELSE
	add.l	__STKDELTA(a4),a1	add on chicken factor
	ENDC

	IFNE	NOBASER
	move.l	a1,__base	set up stack base
	ELSE
	move.l	a1,__base(a4)	set up stack base
	ENDC

	IFNE	NOBASER
	add.l	__STACK,a1
	ELSE
	add.l	__STACK(a4),a1
	ENDC

	move.l	a1,d1		longword align stack for performance on TT
	addq.l	#3,d1
	and.w	#~3,d1
	move.l	d1,a1
	
	cmp.l	a1,a7		compare new stack with current stack
	bcc.s	stk_ok
	move.w	#-39,-(a7)	and abort if invalid
	bra	done
stk_ok
	IFNE	NOBASER
	tst.l	__MSTEP		automatic _mstep sizing ?
	ELSE
	tst.l	__MSTEP(a4)
	ENDC
	
	bne.s	no_amst
	move.l	a7,d1		work out how much we are returning
	sub.l	a1,d1
	lsr.l	#4,d1		set to 1/16 of returned size

	IFNE	NOBASER
	cmp.w	#$104,__tos	tos 1.4 or greater ?
	ELSE
	cmp.w	#$104,__tos(a4)	tos 1.4 or greater ?
	ENDC
	bcs.s	old_tos
	lsr.l	#2,d1		use 1/64 for new tos, be less greedy

old_tos	addq.l	#2,d1		round up (not allowing a zero value)
	bclr	#0,d1		and word align

	IFNE	NOBASER
	move.l	d1,__MSTEP
	ELSE
	move.l	d1,__MSTEP(a4)
	ENDC
	
	IFNE	NOBASER
	move.l	__BLKSIZ,d0		Resize __BLKSIZ for minimal pool spillage
	ELSE
	move.l	__BLKSIZ(a4),d0
	ENDC

	divu	d0,d1
	move.w	d1,d0
	clr.w	d1
	swap	d1
	addq.w	#1,d0

	IFNE	NOBASER
	move.l	__BLKSIZ,d3
	ELSE
	move.l	__BLKSIZ(a4),d3
	ENDC

	sub.l	d1,d3
	divu	d0,d3
	
	swap	d3
	clr.w	d3
	swap	d3

	not.l	d3
	sub.l	#27,d3
	and.w	#$fffc,d3
	
	IFNE	NOBASER
	add.l	d3,__BLKSIZ
	ELSE
	add.l	d3,__BLKSIZ(a4)
	ENDC

no_amst	move.l	a1,a7
	sub.l	a3,d2
	move.l	a3,-(a7)	char *argv[]
	IFNE	SHORTINT
	lsr.w	#2,d2		divide by pointer size
	subq.w	#1,d2		subtract one to correct for NULL we added
	move.w	d2,-(a7) 	int argc
	ELSE
	lsr.l	#2,d2		divide by pointer size
	subq.l	#1,d2		subtract one to correct for NULL we added
	move.l	d2,-(a7) 	int argc
	ENDC
	sub.l	a2,a1		calculate length we need
	add.l	#$100,a1	add in base page
	move.l	a1,-(a7)
	move.l	a2,-(a7)
	move.w	#0,-(a7) 
	move.w	#m_shrink,-(a7)	release memory back to GEM
	trap	#1
	lea	12(a7),a7
	ENDC

	IFNE	AUTO
	or.w	#3,d4			assume auto-folder TOS prog

	moveq	#0,D0
	
	IFNE	NOBASER
	lea	__AESpb,a0
	ELSE
	lea	__AESpb(a4),a0
	ENDC

	move.l	(a0),a1
	move.l	#$0a000100,d0
	movep.l	d0,1(a1)
	move.l	a0,d1
	move.w	#200,d0
	trap	#2

	cmp.w	#200,d0
	beq.s	isada2

	IFNE	NOBASER
	lea	__AESintout,a0
	ELSE
	lea	__AESintout(a4),a0
	ENDC
	move.w	(a0),d0
	bmi.s	aeserr

	IFNE	REGARGS
	IFNE	NOPCREL
	jsr	@appl_exit
	ELSE
	jsr	@appl_exit(pc)
	ENDC
	ELSE
	IFNE	NOPCREL
	jsr	_appl_exit
	ELSE
	jsr	_appl_exit(pc)
	ENDC
	ENDC	

aeserr
	bclr	#1,d4			mark as a regular prog
	dc.w	$a000
	tst.w	-598(a0)
	bne.s	isada2

	bclr	#0,d4			mark as a TOS prog
	
isada2
	IFNE	SHORTINT
	IFNE	NOBASER
	move.w	d4,__XMODE
	ELSE
	move.w	d4,__XMODE(a4)
	ENDC
	ELSE
	IFNE	NOBASER
	move.l	d4,__XMODE
	ELSE
	move.l	d4,__XMODE(a4)
	ENDC
	ENDC
	ENDC
	
	IFNE	NOPCREL
	jsr	__CXSTI		; call global constructors
	jsr	__FPCinit
	IFNE	DA!CPX
	IFNE	CPX
	move.l	(a7)+,a4
	jmp	_cpx_init
	ELSE
	IFNE	REGARGS
	jsr	@main
	ELSE
	jsr	_main
	ENDC
	ENDC
	ELSE
	IFNE	REGARGS
	IFNE	SHORTINT
	move.w	(a7)+,d0
	ELSE
	move.l	(a7)+,d0
	ENDC
	move.l	(a7)+,a0
	jsr	@_main		off to main program
	ELSE
	jsr	__main		off to main program
	ENDC
	ENDC
	ELSE
	jsr	__CXSTI(pc)		; call global constructors
	jsr	__FPCinit(pc)
	IFNE	DA!CPX
	IFNE	CPX
	move.l	(a7)+,a4
	bra.w	_cpx_init
	ELSE
	IFNE	REGARGS
	jsr	@main(pc)
	ELSE
	jsr	_main(pc)
	ENDC
	ENDC
	ELSE
	IFNE	REGARGS
	IFNE	SHORTINT
	move.w	(a7)+,d0
	ELSE
	move.l	(a7)+,d0
	ENDC
	move.l	(a7)+,a0
	jsr	@_main(pc)
	ELSE
	jsr	__main(pc)
	ENDC
	ENDC
	ENDC
	
	IFNE	DA!CPX
	IFNE	REGARGS
@exit
@_exit
@_XCEXIT
	ELSE
_exit
__exit
__XCEXIT
	ENDC
	illegal
	ELSE
	move.w	d0,(a7)
	bra.s	term
	
	IFNE	REGARGS
@_XCEXIT
	move.w	d0,(a7)
	ELSE
__XCEXIT
	IFNE	SHORTINT
	addq.w	#4,a7		move return code to top of stack
	ELSE
	addq.w	#6,a7
	ENDC
	ENDC
	
term
	IFNE	NOPCREL
	jsr	__CXSTD		; call global destructors
	ELSE
	jsr	__CXSTD(pc)		; call global destructors
	ENDC

	IFNE	NOBASER
	move.l	__onbrk,d0
	ELSE
	move.l	__onbrk(a4),d0
	ENDC
	
	beq.s	done

	move.l	d0,-(a7)
	move.w	#$102,-(a7)
	move.w	#5,-(a7)
	trap	#13
	addq.w	#8,a7
	
done	move.w	#p_term,-(a7)
	trap	#1

	illegal

chkstk	cmp.b	#'=',(a6)		stack size specification ?
	bne.s	skp_stk

	addq.w	#1,a6
	moveq	#0,d1

digit
	moveq	#0,d2
	move.b	(a6)+,d2
	sub.w	#$30,d2
	blt.s	donestk
	cmp.w	#9,d2
	bgt.s	donestk
	add.l	d1,d1
	move.l	d1,d3
	add.l	d1,d1
	add.l	d1,d1
	add.l	d3,d1
	add.l	d2,d1
	bra.s	digit
	
donestk	move.b	-1(a6),d2
	beq.s	gstk2
	sub.b	#' ',d2
	beq.s	got_stk
	sub.b	#'K'-' ',d2
	beq.s	times10
	sub.b	#'k'-'K',d2
	bne.s	skp_stk
times10	moveq	#10,d3
	lsl.l	d3,d1
got_stk	addq.w	#1,a6
gstk2	tst.l	d1
	beq.s	skp_stk

	IFNE	NOBASER
	move.l	d1,__STACK
	ELSE
	move.l	d1,__STACK(a4)
	ENDC

	move.l	a6,a0
skp_stk	rts

fixenv
	IFNE	SHORTINT
	moveq	#0,d0
	IFNE	NOBASER
	move.w	__ENVC,d0
	ELSE
	move.w	__ENVC(a4),d0
	ENDC	
	moveq	#0,d1
	IFNE	NOBASER
	move.w	__ENEED,d1
	ELSE
	move.w	__ENEED(a4),d1
	ENDC
	ELSE
	IFNE	NOBASER
	move.l	__ENVC,d0
	ELSE
	move.l	__ENVC(a4),d0
	ENDC	
	IFNE	NOBASER
	move.l	__ENEED,d1
	ELSE
	move.l	__ENEED(a4),d1
	ENDC
	ENDC
	cmp.l	d0,d1
	bcs.s	noteneed
	move.l	d1,d0
noteneed
	add.l	d0,d0
	add.l	d0,d0
	add.l	d0,a1
	rts
	ENDC

superstart
	IFNE	NOBASER
	move.l	(_hz_200).w,__clkstart		start time for clock()
	ELSE
	move.l	(_hz_200).w,__clkstart(a4)
	ENDC
	
	move.l	(_sysbase).w,a0		get _sysbase
	move.l	8(a0),a0		get os_beg
	
	IFNE	NOBASER
	move.w	2(a0),__tos		fetch TOS version number
	ELSE
	move.w	2(a0),__tos(a4)		fetch TOS version number
	ENDC

	move.w	$1c(a0),d1
	lsr.w	#1,d1
	
	IFNE	NOBASER
	IFNE	SHORTINT
	move.w	d1,__country
	ELSE
	move.w	d1,__country+2
	ENDC
	ELSE
	IFNE	SHORTINT
	move.w	d1,__country(a4)
	ELSE
	move.w	d1,__country+2(a4)
	ENDC
	ENDC
	
	IFEQ	DA!CPX
	clr.l	($380).w		so we can spot exceptions
	ENDC
	rts

	IFNE	NOBASER
	CSECT	bss,2
	ELSE
	CSECT	__MERGED,2
	ENDC	

	IFNE	GST

__LinkerDB
	ENDC

_environ	ds.l	1

	IFEQ	DA!CPX
__onbrk	ds.l	1
	ENDC

	IFEQ	CPX	
__pbase
__PBASE	ds.l	1
	ENDC
	
__savePC	ds.l	1
__saveD2A2	ds.l	2
	IFEQ	CPX
__base	ds.l	1
	ENDC
__clkstart	ds.l	1
__msiz	ds.l	1
__mblk	ds.l	1

	IFNE	AUTO
__XMODE
	IFNE	SHORTINT
	ds.w	1
	ELSE
	ds.l	1
	ENDC
	ENDC
	
	IFNE	SHORTINT
__maxblk	ds.w	1
	IFEQ	DA!CPX
__ENVC	ds.w	1
	ENDC
	ELSE
__maxblk	ds.l	1
	IFEQ	DA!CPX
__ENVC	ds.l	1
	ENDC
	ENDC

__OSERR
__oserr	ds.l	1
__tos	ds.w	1
__country	ds.l	1
	END
