				GLOBL	bios
				GLOBL	Bconin
				GLOBL	Bconout
				GLOBL	Bconstat
				GLOBL	Bcostat
				GLOBL	Drvmap
				GLOBL	Getbpb
				GLOBL	Getmpb
				GLOBL	Kbshift
				GLOBL	Mediach
				GLOBL	Rwabs
				GLOBL	Setexc
				GLOBL	Tickcal

				MACRO	CALLBIOS
				trap	#13
				ENDM
				
				
				MODULE	bios
				move.l	save_ptr,a0
				move.l	(a7)+,-(a0)
				move.l	a2,-(a0)
				move.l	a0,save_ptr
				move.w	d0,-(a7)
				CALLBIOS
				addq.l	#2,a7
				move.l	save_ptr,a0
				movea.l	(a0)+,a2
				move.l	(a0)+,-(a7)
				move.l	a0,save_ptr
				rts
				BSS
save_a2_pc:		ds.l	2*8
save_end:
				DATA
save_ptr:		dc.l	save_end
				TEXT
				ENDMOD

				MODULE	Bconin
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#2,-(a7)
				CALLBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Bconout
				pea     (a2)
				move.w	d1,-(a7)
				move.w	d0,-(a7)
				move.w	#3,-(a7)
				CALLBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Bconstat
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#1,-(a7)
				CALLBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Bcostat
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#8,-(a7)
				CALLBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Drvmap
				pea     (a2)
				move.w	#10,-(a7)
				CALLBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Getbpb
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#7,-(a7)
				CALLBIOS
				addq.w	#4,a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Getmpb
				pea     (a2)
				pea     (a0)
				move.w	#0,-(a7)
				CALLBIOS
				addq.w	#6,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Kbshift
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#11,-(a7)
				CALLBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Mediach
				pea     (a2)
				move.w	d0,-(a7)
				move.w	#9,-(a7)
				CALLBIOS
				addq.w	#4,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Rwabs
				pea		(a2)
				move.l  10(a7),-(a7)
				move.w	12(a7),-(a7)
				move.w	d2,-(a7)
				move.w	d1,-(a7)
				pea     (a0)
				move.w	d0,-(a7)
				move.w	#4,-(a7)
				CALLBIOS
				lea		18(a7),a7
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Setexc
				pea		(a2)
				pea		(a0)
				move.w	d0,-(a7)
				move.w	#5,-(a7)
				CALLBIOS
				addq.w	#8,a7
				movea.l	d0,a0
				movea.l	(a7)+,a2
				rts
				ENDMOD

				MODULE	Tickcal
				pea     (a2)
				move.w	#6,-(a7)
				CALLBIOS
				addq.w	#2,a7
				movea.l	(a7)+,a2
				rts
				ENDMOD
