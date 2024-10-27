				INCLUDE	"gem.i"

				GLOBL	fnts_add
				GLOBL	fnts_close
				GLOBL	fnts_create
				GLOBL	fnts_delete
				GLOBL	fnts_do
				GLOBL	fnts_evnt
				GLOBL	fnts_get_info
				GLOBL	fnts_get_name
				GLOBL	fnts_get_no_styles
				GLOBL	fnts_get_style
				GLOBL	fnts_open
				GLOBL	fnts_remove
				GLOBL	fnts_update
				
				MODULE	fnts_create
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.w	4(a7),(a0)
				clr.l	_GemParBlk+addrout
				move.l  #$b4040002,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	fnts_delete
				
				move.w	d0,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l  #$b5010101,d1
				bra		_aes

				ENDMOD


				MODULE	fnts_open
				
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1)
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.l	4(a7),(a1)+
				move.l	8(a7),(a1)+
				move.l	12(a7),(a1)
				move.l	#$b6090101,d1
				bra		_aes
				
				ENDMOD


				MODULE	fnts_close
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,-(a7)
				move.w	#-1,_GemParBlk+aintout+2
				move.w	#-1,_GemParBlk+aintout+4
				move.l	#$b7000301,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	fnts_get_no_styles
				
				move.l	a0,_GemParBlk+addrin
				clr.w	_GemParBlk+aintin
				move.l	d0,_GemParBlk+aintin+2
				move.l	#$b8030101,d1
				bra		_aes
				
				ENDMOD


				MODULE	fnts_get_style
				
				move.l	a0,_GemParBlk+addrin
				move.w	#1,_GemParBlk+aintin
				move.l	d0,_GemParBlk+aintin+2
				move.w	d1,_GemParBlk+aintin+6
				move.l	#$b8040201,d1
				bsr		_aes
				move.l	-2(a0),d0
				rts
				
				ENDMOD


				MODULE	fnts_get_name
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.l	8(a7),_GemParBlk+addrin+12
				move.w	#2,_GemParBlk+aintin
				move.l	d0,_GemParBlk+aintin+2
				move.l	#$b8030104,d1
				bra		_aes
				
				ENDMOD


				MODULE	fnts_get_info
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,-(a7)
				move.w	#3,_GemParBlk+aintin
				move.l	d0,_GemParBlk+aintin+2
				move.l	#$b8030301,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	fnts_add
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#0,_GemParBlk+aintin
				move.l	#$b9010102,d1
				bra		_aes
				
				ENDMOD


				MODULE	fnts_remove
				
				move.l	a0,_GemParBlk+addrin
				move.w	#1,_GemParBlk+aintin
				move.l	#$b9010001,d1
				bra		_aes
				
				ENDMOD


				MODULE	fnts_update
				
				move.l	a0,_GemParBlk+addrin
				lea.l	_GemParBlk+aintin,a0
				move.w	#2,(a0)+
				move.w	d0,(a0)+
				move.l	d1,(a0)+
				move.l	d2,(a0)+
				move.l	4(a7),(a0)
				move.w	#-1,_GemParBlk+aintout
				move.l	#$b9080101,d1
				bra		_aes
				
				ENDMOD


				MODULE	fnts_evnt
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	#$ba000902,d1
				bsr		_aes
				move.l	4(a7),a1
				move.w	(a0)+,(a1)
				move.l	8(a7),a1
				move.w	(a0)+,(a1)
				move.l	12(a7),a1
				move.l	(a0)+,(a1)
				move.l	16(a7),a1
				move.l	(a0)+,(a1)
				move.l	20(a7),a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	fnts_do
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.l	d1,(a0)+
				move.l	d2,(a0)+
				move.l	8(a7),(a0)
				move.l	#$bb070801,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	8(a7),a1
				move.l	(a0)+,(a1)
				move.l	12(a7),a1
				move.l	(a0)+,(a1)
				move.l	16(a7),a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD

