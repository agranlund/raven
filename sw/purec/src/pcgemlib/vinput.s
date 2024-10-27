				INCLUDE "gem.i"

				GLOBL	vex_butv
				GLOBL	vex_curv
				GLOBL	vex_motv
				GLOBL	vex_timv
				GLOBL	vex_wheelv
				GLOBL	vq_key_s
				GLOBL	vq_mouse
				GLOBL	vrq_choice
				GLOBL	vrq_locator
				GLOBL	vrq_string
				GLOBL	vrq_valuator
				GLOBL	vsc_form
				GLOBL	vsin_mode
				GLOBL	vsm_choice
				GLOBL	vsm_locator
				GLOBL	vsm_string
				GLOBL	vsm_valuator
				GLOBL	v_hide_c
				GLOBL	v_show_c


				MODULE	vex_butv

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.l	(a7)+,v_param(a0)
				moveq	#125,d1
				bsr	_VdiCtrl
				movea.l (a7)+,a0
				move.l	_GemParBlk+v_param+4,(a0)
				rts

				ENDMOD


				MODULE	vex_curv

			 	move.l  a1,-(a7)
	         	move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	clr.w   v_nptsin(a0)
	          	clr.w   v_nintin(a0)
	          	move.l  (a7)+,v_param(a0)
	          	moveq   #127,d1
	          	bsr     _VdiCtrl
	         	movea.l (a7)+,a0
	          	move.l  _GemParBlk+v_param+4,(a0)
	          	rts

				ENDMOD


				MODULE	vex_motv

			 	move.l  a1,-(a7)
	          	move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	clr.w   v_nptsin(a0)
	          	clr.w   v_nintin(a0)
	         	move.l  (a7)+,v_param(a0)
	          	moveq   #126,d1
	          	bsr     _VdiCtrl
	          	movea.l (a7)+,a0
	          	move.l  _GemParBlk+v_param+4,(a0)
	          	rts

				ENDMOD


				MODULE	vex_timv

				move.l  a1,-(a7)
	          	move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	clr.w   v_nptsin(a0)
	          	clr.w   v_nintin(a0)
	          	move.l  (a7)+,v_param(a0)
	          	moveq   #118,d1
	          	bsr     _VdiCtrl
	          	movea.l (a7)+,a1
	          	move.l  _GemParBlk+v_param+4,(a1)
	          	movea.l 4(a7),a0
	          	move.w  d0,(a0)
	          	rts

				ENDMOD


				MODULE	vex_wheelv

			 	move.l  a1,-(a7)
	          	move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	clr.w   v_nptsin(a0)
	          	clr.w   v_nintin(a0)
	         	move.l  (a7)+,v_param(a0)
	          	move.w  #134,d1
	          	bsr     _VdiCtrl
	          	movea.l (a7)+,a0
	          	move.l  _GemParBlk+v_param+4,(a0)
	          	rts

				ENDMOD


				MODULE	vq_key_s

				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#128,d1
				bsr	_VdiCtrl
				movea.l (a7)+,a0
				move.w	d0,(a0)
				rts

				ENDMOD


				MODULE	vq_mouse

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				moveq	#124,d1
				bsr	_VdiCtrl
				lea	_GemParBlk+ptsout,a1
				movea.l (a7)+,a0
				move.w	d0,(a0)
				movea.l (a7)+,a0
				move.w	(a1)+,(a0)
				movea.l 4(a7),a0
				move.w	(a1),(a0)
				rts

				ENDMOD


				MODULE	vrq_choice

				move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	clr.w   v_nptsin(a0)
	          	move.w  #1,v_nintin(a0)
	          	move.w  d1,intin(a0)
	          	moveq   #30,d1
	          	bsr     _VdiCtrl
	          	movea.l (a7)+,a0
	          	move.w  d0,(a0)
	          	rts

				ENDMOD


				MODULE	vrq_locator

			 	move.l  a1,-(a7)
	          	move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	move.w  #1,v_nptsin(a0)
	          	clr.w   v_nintin(a0)
	          	move.w  d1,ptsin(a0)
	          	move.w  d2,ptsin+2(a0)
	          	moveq   #28,d1
	          	bsr     _VdiCtrl
	          	lea     _GemParBlk+ptsout,a0
	          	movea.l (a7)+,a1
	          	move.w  (a0)+,(a1)
	          	movea.l (a7)+,a1
	          	move.w  (a0)+,(a1)
	          	movea.l 4(a7),a1
	          	move.w  d0,(a1)
	          	rts

				ENDMOD


				MODULE	 vrq_string

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				movea.l (a7)+,a1
				move.w	(a1)+,ptsin(a0)
				move.w	(a1),ptsin+2(a0)
				moveq	#31,d1
				bsr	_VdiCtrl
				movea.l (a7)+,a0
				lea	_GemParBlk+intout,a1
				move.w	_GemParBlk+v_nintout,d0
				subq.w	#1,d0
				bmi	vrq_string2
vrq_string1:	move.w	(a1)+,d1
				move.b	d1,(a0)+
				dbf	d0,vrq_string1
vrq_string2:	clr.b	(a0)
				rts

				ENDMOD


				MODULE	vrq_valuator

				move.l  a1,-(a7)
	          	move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	clr.w   v_nptsin(a0)
	          	move.w  #1,v_nintin(a0)
	          	move.w  d1,intin(a0)
	          	moveq   #29,d1
	          	bsr     _VdiCtrl
	          	movea.l (a7)+,a1
	          	move.w  d0,(a1)
	          	movea.l (a7)+,a1
	          	move.w  (a0),(a1)
	          	rts

				ENDMOD


				MODULE	vsc_form

				lea	_GemParBlk,a1
				clr.w	v_nptsin(a1)
				move.w	#37,v_nintin(a1)
				lea	intin(a1),a1
				moveq	#37-1,d1
vsc_form1:		move.w	(a0)+,(a1)+
				dbf	d1,vsc_form1
				lea	_GemParBlk,a0
				moveq	#111,d1
				bra	_VdiCtrl

				ENDMOD


				MODULE	vsin_mode

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				moveq	#33,d1
				bra	_VdiCtrl

				ENDMOD


				MODULE	vsm_choice

				move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	clr.w   v_nptsin(a0)
	          	clr.w   v_nintin(a0)
	          	moveq   #30,d1
	          	bsr     _VdiCtrl
	          	movea.l (a7)+,a0
	          	move.w  d0,(a0)
	          	move.w  _GemParBlk+v_nintout,d0
	          	rts

				ENDMOD


				MODULE	vsm_locator

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				moveq	#28,d1
				bsr	_VdiCtrl
				lea	_GemParBlk+ptsout,a0
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	d0,(a1)
				lea	_GemParBlk,a0
				move.w	v_nintout(a0),d0
				add.w	d0,d0
				or.w	v_nptsout(a0),d0
				rts

				ENDMOD


                MODULE    vsm_string

				move.l  a1,-(a7)
	          	move.l  a0,-(a7)
	          	lea     _GemParBlk,a0
	          	move.w  #1,v_nptsin(a0)
	          	move.w  #2,v_nintin(a0)
	          	move.w  d1,intin(a0)
	          	move.w  d2,intin+2(a0)
	          	movea.l (a7)+,a1
	          	move.w  (a1)+,ptsin(a0)
	          	move.w  (a1),ptsin+2(a0)
	          	moveq   #31,d1
	          	bsr     _VdiCtrl
	          	movea.l (a7)+,a0
	          	lea     _GemParBlk+intout,a1
	          	move.w  _GemParBlk+v_nintout,d0
	          	subq.w  #1,d0
	          	bmi     vsm_string2
vsm_string1:	move.w  (a1)+,d1
	          	move.b  d1,(a0)+
	          	dbf     d0,vsm_string1
vsm_string2:	clr.b   (a0)
	          	move.w  _GemParBlk+v_nintout,d0
	          	rts

				ENDMOD


				MODULE	vsm_valuator

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#29,d1
				bsr	_VdiCtrl
				movea.l (a7)+,a1
				move.w	d0,(a1)
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				movea.l 4(a7),a1
				move.w	_GemParBlk+v_nintout,(a1)
				rts

				ENDMOD


				MODULE	v_hide_c

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				moveq	#123,d1
				bra	_VdiCtrl

				ENDMOD


				MODULE	v_show_c

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#122,d1
				bra	_VdiCtrl

				ENDMOD
