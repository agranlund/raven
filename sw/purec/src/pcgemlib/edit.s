				INCLUDE	"gem.i"

				GLOBL	edit_create
				GLOBL	edit_open
				GLOBL	edit_close
				GLOBL	edit_delete
				GLOBL	edit_cursor
				GLOBL	edit_evnt
				
				
				MODULE	edit_create
				
				move.l	#$d2000000,d1
				moveq	#1,d0
				clr.l	_GemParBlk+addrout
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	edit_open
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l	#$d3010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	edit_close
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l	#$d4010001,d1
				bra		_aes
				
				ENDMOD


				MODULE	edit_delete
				
				move.l	a0,_GemParBlk+addrin
				move.l	#$d5000001,d1
				bra		_aes
				
				ENDMOD


				MODULE	edit_cursor
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.w	d1,_GemParBlk+aintin+2
				move.w	d2,_GemParBlk+aintin+4
				move.l	#$d6030101,d1
				bra		_aes
				
				ENDMOD


				MODULE	edit_evnt
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	d0,_GemParBlk+aintin
				move.w	d1,_GemParBlk+aintin+2
				move.l	#$d7020302,d1
				bsr		_aes
				move.l	4(a7),a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD
