				INCLUDE	"gem.i"

				GLOBL	_VdiCtrl
				GLOBL	_VdiCtrl2

				MODULE	_VdiCtrl
				
				clr.w	v_opcode2(a0)
_VdiCtrl2:
				move.l	a2,-(a7)
				move.w	d0,v_handle(a0)
				move.w	d1,v_opcode(a0)
				move.l	#_VdiParBlk,d1
				moveq	#$73,d0
				trap	#2
				lea		_GemParBlk+intout,a0
				move.w	(a0)+,d0
				move.l	(a7)+,a2
				rts

				ENDMOD

				GLOBL	vdi
				MODULE	vdi
								
				move.l	a2,-(a7)
				move.l	a0,d1
				moveq	#$73,d0
				trap	#2
				move.l	(a7)+,a2
				rts

				ENDMOD

			if NEW_MT
; may only be jumped to
; input:
; VDIBP on stack
; returns:
; d1 - nintout
; d0 - intout[0] or zero
; a0 - &intout[1] or NULL
				GLOBL	_mt_vdi_end
				MODULE	_mt_vdi_end
				move.l	sp,d1
				move.l	a2,-(a7)
				moveq	#$73,d0
				trap	#2
				move.l	(a7)+,a2
				moveq	#0,d0
				move.l	v_control(sp),a0
				move.w	v_nintout(a0),d1
				beq.s	_mt_vdi_end1
				move.l	v_intout(sp),a0
				tst.l	a0
				beq.s	_mt_vdi_end1
				move.w	(a0)+,d0
_mt_vdi_end1:	VDI_END
				rts
				ENDMOD

; called as function
; input:
; VDIBP on stack
; may only be jumped to
; returns:
; d1 - nintout
; d0 - intout[0] or zero
; a0 - &intout[1] or NULL
				GLOBL	_mt_vdi
				MODULE	_mt_vdi
				move.l	sp,d1
				addq.l	#4,d1
				move.l	a2,-(a7)
				moveq	#$73,d0
				trap	#2
				move.l	(a7)+,a2
				moveq	#0,d0
				move.l	v_control+4(sp),a0
				move.w	v_nintout(a0),d1
				beq.s	_mt_vdi1
				move.l	v_intout+4(sp),a0
				tst.l	a0
				beq.s	_mt_vdi1
				move.w	(a0)+,d0
_mt_vdi1:		rts
				ENDMOD
			endc
