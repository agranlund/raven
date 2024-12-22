**
** $Header: e:/src/gem/aes/util\_aesif.s,v 1.7 1993/04/02 10:54:00 AGK Exp $
**
** AES binding stub
**
** (c) Copyright 1989, 1993 HiSoft
**
	TEXT
	
	XDEF	@_AESif
	XREF	__AESpb,__AESintout,__AEScontrol
	
@_AESif	movem.l	d2/a2,-(a7)

	IFD	NOBASER
	lea	__AEScontrol,a0
	ELSE
	lea	__AEScontrol(a4),a0
	ENDC
	movep.l	d0,1(a0)
	
	IFD	NOBASER
	move.l	#__AESpb,d1
	ELSE
	lea	__AESpb(a4),a0
	move.l	a0,d1
	ENDC

	move.w	#200,d0
	trap	#2

	IFD	NOBASER
	move.w	__AESintout,d0
	ELSE
	move.w	__AESintout(a4),d0
	ENDC
	
	IFND	SHORTINT
	ext.l	d0
	ENDC

	movem.l	(a7)+,d2/a2
	rts

	END
