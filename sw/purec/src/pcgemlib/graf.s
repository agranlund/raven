				INCLUDE	"gem.i"

				GLOBL	graf_rubberbox,graf_dragbox,graf_movebox,graf_growbox
				GLOBL	graf_shrinkbox,graf_watchbox,graf_slidebox,graf_handle
				GLOBL	graf_mouse,graf_mkstate
				GLOBL   graf_mbox,graf_rubbox
				GLOBL	graf_xhandle
				GLOBL	graf_wwatchbox
				GLOBL	graf_growbox_grect
				GLOBL	graf_shrinkbox_grect
				
				MODULE	graf_rubberbox
				
graf_rubbox:    move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.w	12(a7),(a0)
				move.l  #$46040300,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	graf_multirubber
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.w	8(a7),(a0)
				move.l  #$45040301,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	graf_dragbox
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	12(a7),(a0)+
				move.l	16(a7),(a0)+
				move.w	20(a7),(a0)
				move.l  #$47080300,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				rts
				
				ENDMOD


				MODULE	graf_movebox
				
graf_mbox:		lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	4(a7),(a0)+
				move.w	8(a7),(a0)
				move.l  #$48060100,d1
				bra		_aes

				ENDMOD


				MODULE	graf_growbox
				
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	4(a7),(a0)+
				move.l	8(a7),(a0)+
				move.w	12(a7),(a0)
				move.l  #$49080100,d1
				bra		_aes

				ENDMOD


				MODULE	graf_growbox_grect
				
				move.l	a1,d0
				lea.l	_GemParBlk+aintin,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)+
				move.l	d0,a0
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$49080100,d1
				bra		_aes

				ENDMOD


				MODULE	graf_shrinkbox
				
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	4(a7),(a0)+
				move.l	8(a7),(a0)+
				move.w	12(a7),(a0)
				move.l  #$4a080100,d1
				bra		_aes

				ENDMOD


				MODULE	graf_shrinkbox_grect
				
				move.l	a1,d0
				lea.l	_GemParBlk+aintin,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)+
				move.l	d0,a0
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$4a080100,d1
				bra		_aes

				ENDMOD


				MODULE	graf_watchbox
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d2,_GemParBlk+aintin+2
				move.l  #$4b040101,d1
				bra		_aes

				ENDMOD


				MODULE	graf_wwatchbox
				
				lea		_GemParBlk,a1
				move.l	a0,addrin(a1)
				movem.w	d0-d2,aintin(a1)
				move.w	4(a7),aintin+6(a1)
				move.l  #$3e040101,d1
				bra		_aes

				ENDMOD


				MODULE	graf_slidebox
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d2,_GemParBlk+aintin
				move.l  #$4c030101,d1
				bra		_aes

				ENDMOD


				MODULE	graf_handle
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				move.l  #$4d000500,d1
				bsr		_aes
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


				MODULE	graf_mouse
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$4e010101,d1
				bra		_aes

				ENDMOD


				MODULE	graf_mkstate
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				move.l  #$4f000500,d1
				bsr		_aes
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


				MODULE	graf_mkstate_event
				
				move.l	a0,-(a7)
				move.l  #$4f000500,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	graf_xhandle
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				move.l  #$4d000600,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				movea.l 8(a7),a1
				move.w	(a0)+,(a1)
				movea.l 12(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


