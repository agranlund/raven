				INCLUDE	"gem.i"

				GLOBL	objc_add
				GLOBL	objc_delete
				GLOBL	objc_draw
				GLOBL	objc_draw_grect
				GLOBL	objc_find
				GLOBL	objc_offset
				GLOBL	objc_order
				GLOBL	objc_edit
				GLOBL	objc_change
				GLOBL	objc_change_grect
				GLOBL	objc_sysvar
				GLOBL	objc_xfind
				GLOBL	objc_wchange
				GLOBL	objc_wdraw
				GLOBL	objc_wedit
				GLOBL	objc_xedit
				
				
				MODULE	objc_add
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$28020101,d1
				bra		_aes
				
				ENDMOD


				MODULE	objc_delete
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$29010101,d1
				bra		_aes

				ENDMOD


				MODULE	objc_draw
				
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.l	4(a7),(a1)+
				move.w	8(a7),(a1)+
				move.l  #$2a060101,d1
				bra		_aes

				ENDMOD


				MODULE	objc_draw_grect
				
				move.l	a1,d2
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l	d2,a0
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				move.l  #$2a060101,d1
				bra		_aes

				ENDMOD


				MODULE	objc_wdraw
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk,a1
				move.l	a0,addrin(a1)
				move.l	(a7)+,addrin+4(a1)
				move.w	d0,aintin(a1)
				move.w	d1,aintin+2(a1)
				move.w	d2,aintin+4(a1)
				move.l  #$3c030002,d1
				bra		_aes

				ENDMOD


				MODULE	objc_find
				
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.w	4(a7),(a1)
				move.l  #$2b040101,d1
				bra		_aes

				ENDMOD


				MODULE	objc_offset
				
				move.l	a1,-(a7)
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$2c010301,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	objc_order
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$2d020101,d1
				bra		_aes

				ENDMOD


				MODULE	objc_edit
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l  (a7),a0
				move.w	(a0),(a1)+
				move.w	d2,(a1)+
				move.l  #$2e040201,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	objc_xedit
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.l	8(a7),addrin-aintin+4(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l  (a7),a0
				move.w	(a0),(a1)+
				move.w	d2,(a1)+
				move.l  #$2e040202,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	objc_wedit
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.l  (a7),a0
				move.w	(a0),(a1)+
				move.w	d2,(a1)+
				move.w	8(a7),(a1)+
				move.l  #$41050201,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	objc_change
				
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.l	4(a7),(a1)+
				move.l	8(a7),(a1)+
				move.w	12(a7),(a1)
				move.l  #$2f080101,d1
				bra		_aes

				ENDMOD


				MODULE	objc_change_grect
				
				move.l	a0,_GemParBlk+addrin
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.l	(a1)+,(a0)+
				move.l	(a1),(a0)+
				move.w	d2,(a0)+
				move.w	4(a7),(a0)
				move.l  #$2f080101,d1
				bra		_aes

				ENDMOD


				MODULE	objc_wchange
				
				move.l	a1,-(a7)
				lea.l	_GemParBlk,a1
				move.l	a0,addrin(a1)
				move.l	(a7)+,addrin+4(a1)
				move.w	d0,aintin(a1)
				move.w	d1,aintin+2(a1)
				move.w	d2,aintin+4(a1)
				move.l  #$3d030002,d1
				bra		_aes

				ENDMOD


				MODULE	objc_sysvar
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.w	12(a7),(a1)
				move.l  #$30040300,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	objc_xfind
				
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.w	4(a7),(a1)
				move.l  #$31040101,d1
				bra	_aes

				ENDMOD


