				INCLUDE	"gem.i"

				GLOBL	wind_calc
				GLOBL	wind_close
				GLOBL	wind_create
				GLOBL	wind_delete
				GLOBL	wind_find
				GLOBL	wind_get
				GLOBL	wind_get_grect
				GLOBL	wind_get_int
				GLOBL	wind_get_ptr
				GLOBL	wind_get_str
				GLOBL	wind_xget
				GLOBL	wind_xget_grect
				GLOBL	wind_new
				GLOBL	wind_set
				GLOBL	wind_set_int
				GLOBL	wind_set_str
				GLOBL	wind_set_ptr
				GLOBL	wind_set_ptr_int
				GLOBL	wind_set_grect
				GLOBL	wind_xset
				GLOBL	wind_xset_grect
				GLOBL	wind_update
				GLOBL	wind_open
				GLOBL	wind_open_grect
				GLOBL	wind_draw
				GLOBL	wind_create_grect
				GLOBL	wind_calc_grect
				GLOBL	wind_xcreate_grect
				


				MODULE	wind_create
				
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	4(a7),(a0)
				move.l  #$64050100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_create_grect
				
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$64050100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_xcreate_grect
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$64050100,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	wind_open
				
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	4(a7),(a0)
				move.l  #$65050100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_open_grect
				
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$65050100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_close
				
				move.w	d0,_GemParBlk+aintin
				move.l  #$66010100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_delete
				
				move.w	d0,_GemParBlk+aintin
				move.l  #$67010100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_get
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a1
				clr.l	aintout+2-aintin(a1)
				clr.l	aintout+6-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				cmp.w #2,d1		; WF_NAME ?
				beq wind_gptr
				cmp.w #3,d1		; WF_INFO ?
				beq wind_gptr
				cmp.w #19,d1		; WF_DCOLOR ?
				beq wind_gcolor
				cmp.w #18,d1		; WF_COLOR ?
				beq wind_gcolor
				move.l	#$68020500,d1
wind_get0:
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_get1
				move.w	d1,(a1)
wind_get1:		movea.l (a7)+,a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_get2
				move.w	d1,(a1)
wind_get2:		movea.l 4(a7),a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_get3
				move.w	d1,(a1)
wind_get3:		movea.l 8(a7),a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_get4
				move.w	d1,(a1)
wind_get4:		rts
wind_gptr:
				; WF_NAME or WF_INFO; do not clobber output
				move.l a0,(a1)
				move.l	#$68040500,d1 ; 4 inputs
				bsr _aes
				addq.l #8,a7
				rts
wind_gcolor:
				; WF_COLOR or WF_DCOLOR:
				; output parameter *gw1 contains the
				; component to query on input
				move.w (a0),(a1)
				move.l	#$68030500,d1 ; 3 inputs
				bra wind_get0

				ENDMOD


				MODULE	wind_get_int
				
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	#$68020500,d1
				cmp.w #19,d1		; WF_DCOLOR ?
				beq windint_gcolor
				cmp.w #18,d1		; WF_COLOR ?
				beq windint_gcolor
windint_nogcolor:
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts
windint_gcolor:
				move.w (a0),(a1)
				move.l	#$68030500,d1 ; 3 inputs
				bra windint_nogcolor

				ENDMOD


				MODULE	wind_get_ptr
				
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a1
				clr.l	aintout+2-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	#$68020500,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	wind_get_str
				
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l  a0,(a1)
				move.l	#$68020500,d1
				bra		_aes
				
				ENDMOD


				MODULE	wind_get_grect
				
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a0
				clr.l   aintout+2-aintin(a0)
				clr.l   aintout+6-aintin(a0)
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.l	#$68020500,d1
				bsr		_aes
				move.l  (a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	wind_xget
				
				move.l	a1,d2
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	(a0),(a1)+
				move.l	d2,a0
				move.w	(a0),(a1)+
				movea.l 4(a7),a0
				move.w	(a0),(a1)+
				movea.l 8(a7),a0
				move.w	(a0),(a1)+
				move.l	#$68060500,d1
				bsr		_aes
				movea.l 12(a7),a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_xget1
				move.w	d1,(a1)
wind_xget1:		movea.l 16(a7),a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_xget2
				move.w	d1,(a1)
wind_xget2:		movea.l 20(a7),a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_xget3
				move.w	d1,(a1)
wind_xget3:		movea.l 24(a7),a1
				move.w	(a0)+,d1
				tst.l	a1
				beq.s	wind_xget4
				move.w	d1,(a1)
wind_xget4:		rts
				
				ENDMOD


				MODULE	wind_xget_grect
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l	#$68060500,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	wind_set
				
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				; move.l	4(a7),(a0)+
				; move.l	8(a7),(a0)
				move.w	d2,(a0)+
				move.l	4(a7),(a0)+
				move.w	8(a7),(a0)
				move.l  #$69060100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_set_int
				
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				clr.w	d2
				move.w	d2,(a0)+
				move.w	d2,(a0)+
				move.w	d2,(a0)+
				move.l  #$69060100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_set_grect
				
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$69060100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_set_str
				
wind_set_ptr:
				moveq	#0,d2
wind_set_ptr_int:
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	a0,(a1)+
				move.w	d2,(a1)+
				clr.w	(a1)
				move.l  #$69060100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_xset
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	12(a7),(a0)+
				move.w	16(a7),(a0)
				move.l  #$69060500,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,d0
				tst.l	a1
				beq.s	wind_xset1
				move.w	d0,(a1)
wind_xset1:		move.l	(a7)+,a1
				move.w	(a0)+,d0
				tst.l	a1
				beq.s	wind_xset2
				move.w	d0,(a1)
wind_xset2:		movea.l 10(a7),a1
				move.w	(a0)+,d0
				tst.l	a1
				beq.s	wind_xset3
				move.w	d0,(a1)
wind_xset3:		movea.l 14(a7),a1
				move.w	(a0)+,d0
				tst.l	a1
				beq.s	wind_xset4
				move.w	d0,(a1)
wind_xset4:		rts
				
				ENDMOD


				MODULE	wind_xset_grect
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$69060500,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	wind_find
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$6a020100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_update
				
				move.w	d0,_GemParBlk+aintin
				move.l  #$6b010100,d1
				bra		_aes

				ENDMOD


				MODULE	wind_calc
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	12(a7),(a0)+
				move.w	16(a7),(a0)
				move.l  #$6c060500,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 10(a7),a1
				move.w	(a0)+,(a1)
				movea.l 14(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	wind_calc_grect
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$6c060500,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	wind_new
				
				move.l  #$6d000000,d1
				bra		_aes

				ENDMOD


				MODULE	wind_draw
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$63020100,d1
				bra		_aes

				ENDMOD
