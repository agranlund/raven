/*
	These routines are the actual interface to the OS.  They save registers
	and perfrom the apporpriate trap
*/

#define REENTER 4
long bios(), xbios(), gemdos();
long _save[REENTER*3];	/* place to save registers */
int _saveSP;

#ifndef LINT
#ifdef PCREL
asm {
	bios:
		move.l	A4, A0
		adda.w	_saveSP(A4), A0
		addq.w	#4, _saveSP(A4)
		move.l	A1, _save(A0)
		move.l	A2, _save+(REENTER*4)(A0)
		move.l	(A7)+, _save+(REENTER*4*2)(A0)
		trap	#13
		move.l	A4, A0
		subq.w	#4, _saveSP(A4)
		adda.w	_saveSP(A4), A0
		move.l	_save(A0), A1
		move.l	_save+(REENTER*4)(A0), A2
		move.l	_save+(REENTER*4*2)(A0), -(A7)
		rts
	xbios:
		move.l	A4, A0
		adda.w	_saveSP(A4), A0
		addq.w	#4, _saveSP(A4)
		move.l	A1, _save(A0)
		move.l	A2, _save+(REENTER*4)(A0)
		move.l	(A7)+, _save+(REENTER*4*2)(A0)
		trap	#14
		move.l	A4, A0
		subq.w	#4, _saveSP(A4)
		adda.w	_saveSP(A4), A0
		move.l	_save(A0), A1
		move.l	_save+(REENTER*4)(A0), A2
		move.l	_save+(REENTER*4*2)(A0), -(A7)
		rts
	gemdos:
		move.l	A4, A0
		adda.w	_saveSP(A4), A0
		addq.w	#4, _saveSP(A4)
		move.l	A1, _save(A0)
		move.l	A2, _save+(REENTER*4)(A0)
		move.l	(A7)+, _save+(REENTER*4*2)(A0)
		trap	#1
		move.l	A4, A0
		subq.w	#4, _saveSP(A4)
		adda.w	_saveSP(A4), A0
		move.l	_save(A0), A1
		move.l	_save+(REENTER*4)(A0), A2
		move.l	_save+(REENTER*4*2)(A0), -(A7)
		rts
}
#else
asm {
	bios:
		lea		_save, A0
		adda.w	_saveSP, A0
		addq.w	#4, _saveSP
		move.l	A1, (A0)
		move.l	A2, REENTER*4(A0)
		move.l	(A7)+, REENTER*4*2(A0)
		trap	#13
		subq.w	#4, _saveSP
		lea		_save, A0
		adda.w	_saveSP, A0
		move.l	(A0), A1
		move.l	REENTER*4(A0), A2
		move.l	REENTER*4*2(A0), -(A7)
		tst     D0
		rts
	xbios:
		lea		_save, A0
		adda.w	_saveSP, A0
		addq.w	#4, _saveSP
		move.l	A1, (A0)
		move.l	A2, REENTER*4(A0)
		move.l	(A7)+, REENTER*4*2(A0)
		trap	#14
		subq.w	#4, _saveSP
		lea		_save, A0
		adda.w	_saveSP, A0
		move.l	(A0), A1
		move.l	REENTER*4(A0), A2
		move.l	REENTER*4*2(A0), -(A7)
		tst		D0
		rts
	gemdos:
		lea		_save, A0
		adda.w	_saveSP, A0
		addq.w	#4, _saveSP
		move.l	A1, (A0)
		move.l	A2, REENTER*4(A0)
		move.l	(A7)+, REENTER*4*2(A0)
		trap	#1
		subq.w	#4, _saveSP
		lea		_save, A0
		adda.w	_saveSP, A0
		move.l	(A0), A1
		move.l	REENTER*4(A0), A2
		move.l	REENTER*4*2(A0), -(A7)
		tst     D0
		rts
}
#endif
#endif
