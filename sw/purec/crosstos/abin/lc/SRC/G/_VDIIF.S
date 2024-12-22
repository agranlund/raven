**
** $Header: e:/src/gem/vdi/util\_vdiif.s,v 1.4 1993/04/06 12:53:02 AGK Exp $
**
** VDI interface stub
**
** (c) Copyright 1989, 1993 HiSoft
**
	TEXT
	XDEF	@_VDIif
	XREF	__VDIpb
	
@_VDIif:
	IFD	NOBASER
	lea	__VDIpb,a0
	ELSE
	lea	__VDIpb(a4),a0
	ENDC

	move.l	(a0),a1

	move.l	d1,(a1)
	move.w	d0,6(a1)
	swap	d0
	move.w	d0,12(a1)
	move.l	a0,d1
	moveq	#115,d0
	trap	#2
	rts
	END
