#ifdef PCREL

/*
	This is the very first stuff that executes when a program is run.
	It sets up the global variables, strings, stack etc.
*/

#define STACKSIZE 8192

/* Base page definitions */
#define ltpa 0			/* Low TPA address */
#define htpa 4			/* High TPA address */
#define lcode 8			/* Code segment start */
#define codelen 12		/* Code segment length */
#define ldata 16		/* Data segment start */
#define datalen 20		/* Data segment length */
#define lbss 24			/* Bss  segment start */
#define bsslen 28		/* Bss  segment length */
#define freelen 32		/* free segment length */
#define par_pd 36		/* Parent basepage */
#define environment 44  /* environment pointer */
#define fcb2 56			/* 2nd parsed fcb */
#define fcb1 92			/* 1st parsed fcb */
#define command 128		/* Command tail */

extern _main(), _init(), _initargcv(), main(), exit(), _begin_profile();
char *_base;	/* points to base page of program */

#ifndef LINT
overlay "init!"
asm {
	_init:		/* initialization entry point; RTS plugged in by linker */
}
#endif

int _argc;		/* initialized by _initargcv */
char **_argv;
char **environ;

int errno;	/* UNIX error global */
long _seed;

#ifndef LINT
overlay "main"
asm {
	_main:
		move.l	A7,A5			; save A7 so we can get the base page address
		move.l	4(A5),A5		; A5=basepage address
		move.l	codelen(A5),D0
		add.l	datalen(A5),D0
		add.l	bsslen(A5),D0
		add.l	#256+STACKSIZE,D0 ;D0=basepage+textlen+datalen+bsslen+8K stack
		move.l	D0,D1
		add.l	A5,D1			; compute stack top
		and.l	#-2,D1			; ensure even byte boundary
		move.l	D1,A7			; setup user stack

		move.l	D0,-(A7)		; return storage above stack to system
		move.l	A5,-(A7)
		clr.w	-(A7)			; junk word
		move	#0x4a,-(A7)		; return excess storage
		trap	#1
		adda.l	#12,A7

		move.l	lbss(A5), A0	;swap data and bss segments
		move.l	lbss(A5), A1
		adda.l	bsslen(A5), A1
		move.l	datalen(A5), D0
		subq.l	#1, D0			; dbf loop is stupid
		ble		swapcont		;might not have any data
	swap:
		move.b	-(A0), -(A1)
		dbf		D0,swap
	swapcont:

		move.l	ldata(A5), A0	;clear bss segment
		move.l	bsslen(A5), D0
		subq.l	#1, D0
	clear:	
		clr.b	(A0)+
		dbf		D0, clear

		move.l	ldata(A5), A4	;A4 points between bss and data
		adda.l	bsslen(A5), A4

		move.l	A5, _base(A4);

		move.l	lcode(A5), A5	;A5 points to jump table

		jsr		_init			;initialize globals and statics now

		move.l	_base(A4), A0	;set up argc and argv parameters
		move.l	environment(A0), -(A7)
		pea		command(A0)
		jsr		_initargcv		;parse command line into argc and argv
		addq.l	#8, A7
		move.l	environ(A4), -(A7)
		move.l	_argv(A4),-(A7)
		move.w	_argc(A4),-(A7)
		;jsr		_begin_profile(PC)	;normally a dummy procedure
		jsr		main			;call applications entry point
		addq.l	#10, A7

		move	#0, -(A7)
		jsr		exit

notify:	/**** DEBUGING stuff ****/
	move	#65, -(A7)
	move	#2, -(A7)
	trap	#1
	addq.l	#4, A7
	rts
}
#endif

#else
/************************ ABSOLUTE VERSION ********************************/

/*
	This is the very first stuff that executes when a program is run.
	It sets up the global variables, strings, stack etc.
*/

#define STACKSIZE 8192

/* Base page definitions */
#define ltpa 0			/* Low TPA address */
#define htpa 4			/* High TPA address */
#define lcode 8			/* Code segment start */
#define codelen 12		/* Code segment length */
#define ldata 16		/* Data segment start */
#define datalen 20		/* Data segment length */
#define lbss 24			/* Bss  segment start */
#define bsslen 28		/* Bss  segment length */
#define freelen 32		/* free segment length */
#define par_pd 36		/* Parent basepage */
#define environment 44	/* environment pointer */
#define fcb2 56			/* 2nd parsed fcb */
#define fcb1 92			/* 1st parsed fcb */
#define command 128		/* Command tail */

extern _main(), _initargcv(), main(), exit(), _begin_profile(), _dbuginit();
char *_base;	/* points to base page of program */
int _app = 1;	/* 1=program is app, 0=program is accessory */

int _argc;		/* initialized by _initargcv */
char **_argv;
char **environ;
static char stack[100];

/*
	Moved these globals to init code.  rpt.  8-20-87
*/
int errno;	/* UNIX error global */
long _seed;
extern long _stksize;

extern notify();

#ifndef LINT
asm {
	_main:
		lea		_main, A5		; Compute basepage address
		suba.l	#256, A5

		tst.l   par_pd(A5)      ; Parent basepage pointer clear if ACC
		bne     app

	acc:						; Its a desk accessory if it isnt
		clr		_app			; Tell user its an accessory
		lea		stack+100, A7
		move.l	_stksize, -(A7)
		move	#0x48, -(A7)
		trap	#1				; Malloc space for the stack
		addq	#6, A7
		tst.l	D0
	self:
		beq		self			; Cant get stack, just loop infinitely
		move.l	D0, A7
		adda.l	_stksize, A7
		bra 	cont

	app:
		move.l	codelen(A5),D0
		add.l	datalen(A5),D0
		add.l	bsslen(A5),D0
		add.l	_stksize, D0
		addi.l	#256,D0
		move.l	D0,D1
		add.l	A5,D1			; compute stack top
		and.l	#-2,D1			; ensure even byte boundary
		move.l	D1,A7			; setup user stack

		move.l	D0,-(A7)		; return storage above stack to system
		move.l	A5,-(A7)
		clr.w	-(A7)			; junk word
		move	#0x4a,-(A7)		; return excess storage
		trap	#1
		adda.l	#12,A7

	cont:
		move.l	A5, _base;

		move.l	_base, A0		;set up argc and argv parameters
		move.l	environment(A0), -(A7)
		pea		command(A0)
		jsr		_initargcv		;parse command line into argc and argv
		addq.l	#8, A7
		move.l	environ,-(A7)
		move.l	_argv,-(A7)
		move.w	_argc,-(A7)
		;jsr		_begin_profile	;normally a dummy procedure
		jsr		main			;call applications entry point
		addq.l	#10, A7

		move	#0, -(A7)
		jsr		exit
}
#endif
#endif
