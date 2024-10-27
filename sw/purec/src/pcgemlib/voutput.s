				INCLUDE "gem.i"

				GLOBL	vr_recfl
				GLOBL	v_arc
				GLOBL	v_bar
				GLOBL	v_cellarray
				GLOBL	v_circle
				GLOBL	v_contourfill
				GLOBL	v_ellarc
				GLOBL	v_ellipse
				GLOBL	v_ellpie
				GLOBL	v_fillarea
				GLOBL	v_gtext
				GLOBL	v_gtextn
				GLOBL	v_gtext16
				GLOBL	v_gtext16n
				GLOBL	v_justified
				GLOBL	v_justified16
				GLOBL	v_justified16n
				GLOBL	v_pieslice
				GLOBL	v_pline
				GLOBL	v_pmarker
				GLOBL	v_rbox
				GLOBL	v_rfbox
				GLOBL	v_bez_on
				GLOBL	v_bez_off
				GLOBL	v_bez_con
				GLOBL	v_setrgb

				MODULE	vr_recfl

				lea		_GemParBlk+ptsin,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				moveq	#114,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_arc

				lea		_GemParBlk,a0
				move.w	#4,v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	#2,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				clr.w	ptsin+4(a0)
				clr.w	ptsin+6(a0)
				clr.w	ptsin+8(a0)
				clr.w	ptsin+10(a0)
				move.w	4(a7),ptsin+12(a0)
				clr.w	ptsin+14(a0)
				move.w	6(a7),intin(a0)
				move.w	8(a7),intin+2(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_bar

				lea		_GemParBlk+ptsin,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#1,v_opcode2(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_cellarray

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				move.w	d1,v_param(a0)
				move.w	d2,v_param+2(a0)
				move.w	12(a7),v_param+4(a0)
				move.w	14(a7),v_param+6(a0)
				lea		ptsin(a0),a1
				movea.l (a7)+,a0
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				movea.l (a7)+,a1
				lea		_GemParBlk+intin,a0
				move.w	4(a7),d1
				mulu	d2,d1
				subq.w	#1,d1
v_cellarray1:
				move.w	(a1)+,(a0)+
				dbf		d1,v_cellarray1
				lea		_GemParBlk,a0
				moveq	#10,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_circle

				lea		_GemParBlk,a0
				move.w	#3,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#4,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				clr.l	ptsin+4(a0)
				move.w	4(a7),ptsin+8(a0)
				clr.w	ptsin+10(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_contourfill

				lea		_GemParBlk,a0
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				move.w	#1,v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	4(a7),intin(a0)
				moveq	#103,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_ellarc

				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	#6,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				move.l	4(a7),ptsin+4(a0)
				move.l	8(a7),intin(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_ellipse

				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#5,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				move.l	4(a7),ptsin+4(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_ellpie

				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	#7,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				move.l	4(a7),ptsin+4(a0)
				move.l	8(a7),intin(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_fillarea

				lea		_GemParBlk,a1
				move.w	d1,v_nptsin(a1)
				clr.w	v_nintin(a1)
				move.l	a0,_VdiParBlk+v_ptsin
				movea.l a1,a0
				moveq	#9,d1
				bsr		_VdiCtrl
				move.l	#_GemParBlk+ptsin,_VdiParBlk+v_ptsin
				rts

				ENDMOD


				MODULE	v_gtext

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				lea		intin(a1),a1
				moveq	#0,d1
				moveq	#-1,d2
v_gtext1:		move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_gtext1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#8,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_gtext16

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				lea		intin(a1),a1
				moveq	#0,d1
				moveq	#-1,d2
v_gtext161:		move.w	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_gtext161
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#8,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_gtextn

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				moveq	#0,d1
				move.w	4(a7),d2
				move.w	d2,v_nintin(a1)
				lea		intin(a1),a1
				tst.w	d2
				beq.s	v_gtextn2
v_gtextn1:		move.b	(a0)+,d1
				move.w	d1,(a1)+
				subq.w	#1,d2
				bne.s	v_gtextn1
v_gtextn2:		clr.w	(a1)
				lea		_GemParBlk,a0
				moveq	#8,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_gtext16n

				lea		_GemParBlk,a1
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	#1,v_nptsin(a1)
				moveq	#0,d1
				move.w	4(a7),d2
				move.w	d2,v_nintin(a1)
				move.l	a0,_VdiParBlk+v_intin
				lea		_GemParBlk,a0
				moveq	#8,d1
				bsr		_VdiCtrl
				move.l	#_GemParBlk+intin,_VdiParBlk+v_intin
				rts

				ENDMOD


				MODULE	v_justified

				lea		_GemParBlk,a1
				move.w	#2,v_nptsin(a1)
				move.w	#10,v_opcode2(a1)
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	4(a7),ptsin+4(a1)
				clr.w	ptsin+6(a1)
				lea		intin(a1),a1
				move.l	6(a7),(a1)+
				moveq	#0,d1
				moveq	#1,d2
v_justified1:	move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_justified1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_justified16

				lea		_GemParBlk,a1
				move.w	#2,v_nptsin(a1)
				move.w	#10,v_opcode2(a1)
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w	4(a7),ptsin+4(a1) /* len */
				clr.w	ptsin+6(a1)
				lea		intin(a1),a1
				move.l	6(a7),(a1)+  /* word_space & char_space */
				moveq	#0,d1
				moveq	#1,d2
v_justified16_1:	move.w	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_justified16_1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_justified16n

				lea		_GemParBlk,a1
				move.w	#2,v_nptsin(a1)
				move.w	#10,v_opcode2(a1)
				move.w	d1,ptsin(a1)
				move.w	d2,ptsin+2(a1)
				move.w  4(a7),d2	 /* num */
				move.w	6(a7),ptsin+4(a1) /* len */
				clr.w	ptsin+6(a1)
				move.w	d2,d1
				addq.w  #2,d1
				move.w	d1,v_nintin(a1)
				lea		intin(a1),a1
				move.l	8(a7),(a1)+  /* word_space & char_space */
				bra.s	v_justified16n_2
v_justified16n_1:
				move.w	(a0)+,(a1)+
v_justified16n_2:
				dbra	d2,v_justified16n_1
				lea		_GemParBlk,a0
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_pieslice

				lea		_GemParBlk,a0
				move.w	#4,v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	#3,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				clr.l	ptsin+4(a0)
				clr.l	ptsin+8(a0)
				move.w	4(a7),ptsin+12(a0)
				clr.w	ptsin+14(a0)
				move.l	6(a7),intin(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_pline

				lea		_GemParBlk,a1
				move.w	d1,v_nptsin(a1)
				clr.w	v_nintin(a1)
				move.l	a0,_VdiParBlk+v_ptsin
				movea.l a1,a0
				moveq	#6,d1
				bsr		_VdiCtrl
				move.l	#_GemParBlk+ptsin,_VdiParBlk+v_ptsin
				rts

				ENDMOD


				MODULE	v_pmarker

				lea		_GemParBlk,a1
				move.w	d1,v_nptsin(a1)
				clr.w	v_nintin(a1)
				move.l	a0,_VdiParBlk+v_ptsin
				movea.l a1,a0
				moveq	#7,d1
				bsr		_VdiCtrl
				move.l	#_GemParBlk+ptsin,_VdiParBlk+v_ptsin
				rts

				ENDMOD


				MODULE	v_rbox

			 	lea		_GemParBlk+ptsin,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#8,v_opcode2(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_rfbox

				lea		_GemParBlk+ptsin,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#9,v_opcode2(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_bez_on

				moveq	#1,d1
				bra		v_bez_con

				ENDMOD


				MODULE	v_bez_off

				moveq	#0,d1
				bra		v_bez_con

				ENDMOD


				MODULE	v_bez_con

				lea		_GemParBlk,a0
				move.w	d1,v_nptsin(a0) ; nptsin is ON/OFF flag!
				clr.w	v_nintin(a0)
				clr.w	intout(a0)
				move.w	#13,v_opcode2(a0)
				moveq	#11,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_setrgb

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#3,v_nintin(a0)
				move.w	d1,v_opcode2(a0)
				move.w	d2,intin(a0)
				move.l	4(a7),intin+2(a0)
				move.w	#138,d1
				bra		_VdiCtrl2

				ENDMOD
