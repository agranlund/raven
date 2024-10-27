				INCLUDE "gem.i"

				GLOBL	vst_arbpt
				GLOBL	vst_arbpt32
				GLOBL	vqt_advance
				GLOBL	vqt_advance32
				GLOBL	vst_skew
				GLOBL	vst_scratch
				GLOBL	vst_error
				GLOBL	vst_setsize
				GLOBL	vst_setsize32
				GLOBL	vqt_f_extent
				GLOBL	vqt_f_extentn
				GLOBL	v_ftext
				GLOBL	v_ftextn
				GLOBL	v_ftext16
				GLOBL	v_ftext16n
				GLOBL	vqt_real_extent
				GLOBL	vqt_real_extentn
				GLOBL	v_ftext_offset
				GLOBL	v_ftext_offset16
				
				
				MODULE	vst_scratch

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#244,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_arbpt

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w   intout(a0)
				move.w	#1,v_nintin(a0)
				clr.w	ptsin(a0)
				move.w	d1,intin(a0)
				move.w	#246,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				movea.l 8(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD

				MODULE	vst_arbpt32

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.l   intout(a0)
				move.w	#2,v_nintin(a0)
				clr.w	ptsin(a0)
				move.l	d1,intin(a0)
				move.w	#246,d1
				bsr		_VdiCtrl
				move.l	-2(a0),d0
				cmp.w	#2,_GemParBlk+v_nintout
				bge		vst_arb1
				clr.l	d0
vst_arb1:		lea		_GemParBlk+ptsout,a0
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				movea.l 8(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD

				MODULE	vst_skew

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#253,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_error

				lea		_GemParBlk,a1
				clr.w	v_nptsin(a1)
				move.w	#3,v_nintin(a1)
				move.w	d1,intin(a1)
				move.l	a0,intin+2(a1)
				move.w	#245,d1
				move.l	a1,a0
				bra		_VdiCtrl

				ENDMOD

				
				MODULE	vst_setsize

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a1
				clr.w	v_nptsin(a1)
				move.w	#1,v_nintin(a1)
				move.w	d1,intin(a1)
				move.w	#252,d1
				move.l	a1,a0
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0)+,(a1)
				move.l	8(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	vst_setsize32

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a1
				clr.w	v_nptsin(a1)
				move.w	#2,v_nintin(a1)
				move.l	d1,intin(a1)
				move.w	#252,d1
				move.l	a1,a0
				bsr		_VdiCtrl
				move.l	-2(a0),d0
				cmp.w	#2,_GemParBlk+v_nintout
				bge		vst_setsize1
				clr.w	d0
vst_setsize1:	lea		_GemParBlk+ptsout,a0
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0)+,(a1)
				move.l	8(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	vqt_f_extent

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				lea		intin(a0),a0
				moveq	#0,d1
				moveq	#0,d2
vqt_f_extent1:	move.b	(a1)+,d1
				beq		vqt_f_extent2
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra		vqt_f_extent1
vqt_f_extent2:	lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#240,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				GLOBL	vqt_f_extent16
				MODULE	vqt_f_extent16

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				lea		intin(a0),a0
				moveq	#0,d1
				moveq	#0,d2
vqt_f_ext161:	move.w	(a1)+,d1
				beq		vqt_f_ext162
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra		vqt_f_ext161
vqt_f_ext162:	lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#240,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	vqt_f_extentn

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	d1,v_nintin(a0)
				lea		intin(a0),a0
				moveq	#0,d2
				tst.w	d1
				beq		vqt_f_extentn2
vqt_f_extentn1:	move.b	(a1)+,d2
				move.w	d2,(a0)+
				subq.w	#1,d1
				bne.s	vqt_f_extentn1
vqt_f_extentn2:	clr.w	(a0)
				lea		_GemParBlk,a0
				move.w	#240,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				GLOBL vqt_f_extent16n
				MODULE	vqt_f_extent16n

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	d1,v_nintin(a0)
				lea		intin(a0),a0
				moveq	#0,d2
				tst.w	d1
				beq		vqt_f_ext16n2
vqt_f_ext16n1:	move.w	(a1)+,d2
				move.w	d2,(a0)+
				subq.w	#1,d1
				bne.s	vqt_f_ext16n1
vqt_f_ext16n2:	clr.w	(a0)
				lea		_GemParBlk,a0
				move.w	#240,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	v_ftext

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				lea		intin(a1),a1
				moveq	#0,d1
				moveq	#-1,d2
v_ftext1:		move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_ftext1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#241,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_ftext16

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				lea		intin(a1),a1
				moveq	#0,d1
				moveq	#-1,d2
v_ftext161:		move.w	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_ftext161
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#241,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_ftextn

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				moveq	#0,d1
				move.w	4(a7),d2
				move.w	d2,v_nintin(a1)
				lea		intin(a1),a1
				tst.w	d2
				beq.s	v_ftextn2
v_ftextn1:		move.b	(a0)+,d1
				move.w	d1,(a1)+
				subq.w	#1,d2
				bne.s	v_ftextn1
v_ftextn2:		clr.w	(a1)
				lea		_GemParBlk,a0
				move.w	#241,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_ftext16n

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				moveq	#0,d1
				move.w	4(a7),d2
				move.w	d2,v_nintin(a1)
				lea		intin(a1),a1
				tst.w	d2
				beq.s	v_ftext16n2
v_ftext16n1:	move.w	(a0)+,d1
				move.w	d1,(a1)+
				subq.w	#1,d2
				bne.s	v_ftext16n1
v_ftext16n2:	clr.w	(a1)
				lea		_GemParBlk,a0
				move.w	#241,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vqt_real_extent

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				lea		intin(a0),a0
				moveq	#0,d1
				moveq	#0,d2
vqt_real_ext1:	move.b	(a1)+,d1
				beq		vqt_real_ext2
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra		vqt_real_ext1
vqt_real_ext2:	clr.w	(a0)
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#240,d1
				move.w	#4200,v_opcode2(a0)
				bsr		_VdiCtrl2
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD



				MODULE	vqt_real_extentn

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				move.w	8(a7),d2
				move.w	d2,v_nintin(a0)
				lea		intin(a0),a0
				moveq	#0,d1
				tst.w	d2
				beq		vqtn_real_ext2
vqtn_real_ext1:	move.b	(a1)+,d1
				move.w	d1,(a0)+
				subq.w	#1,d2
				bne		vqtn_real_ext1
vqtn_real_ext2:	clr.w	(a0)
				lea		_GemParBlk,a0
				move.w	#240,d1
				move.w	#4200,v_opcode2(a0)
				bsr		_VdiCtrl2
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD



				MODULE	v_ftext_offset

				move.l	a1,-(a7)
				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				lea		intin(a1),a1
				moveq	#0,d1
				moveq	#-1,d2
v_ftext_offset1:move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_ftext_offset1
				move.l	(a7)+,a1
				move.w	d2,d1
				lea		_GemParBlk+ptsin+4,a0
				bra		v_ftext_offset3
v_ftext_offset2:move.l	(a1)+,(a0)+
v_ftext_offset3:dbf		d1,v_ftext_offset2
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				addq.w	#1,d2
				move.w	d2,v_nptsin(a0)
				move.w	#241,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_ftext_offset16

				move.l	a1,-(a7)
				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				lea		intin(a1),a1
				moveq	#0,d1
				moveq	#-1,d2
v_ftext_offset161:move.w	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_ftext_offset161
				move.l	(a7)+,a1
				move.w	d2,d1
				lea		_GemParBlk+ptsin+4,a0
				bra		v_ftext_offset163
v_ftext_offset162:move.l	(a1)+,(a0)+
v_ftext_offset163:dbf		d1,v_ftext_offset162
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				addq.w	#1,d2
				move.w	d2,v_nptsin(a0)
				move.w	#241,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vqt_advance

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				clr.w	ptsin(a0)
				move.w	d1,intin(a0)
				move.w	#247,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				movea.l 8(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	vqt_advance32

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				clr.w	ptsin(a0)
				move.w	d1,intin(a0)
				move.w	#247,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout+8,a0
				movea.l (a7)+,a1
				move.l	(a0)+,(a1)
				movea.l (a7)+,a1
				move.l	(a0),(a1)
				rts

				ENDMOD
