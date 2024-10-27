				INCLUDE	"gem.i"

				GLOBL	wdlg_close
				GLOBL	wdlg_create
				GLOBL	wdlg_delete
				GLOBL	wdlg_evnt
				GLOBL	wdlg_get_edit
				GLOBL	wdlg_get_handle
				GLOBL	wdlg_get_tree
				GLOBL	wdlg_get_udata
				GLOBL	wdlg_open
				GLOBL	wdlg_redraw
				GLOBL	wdlg_set_edit
				GLOBL	wdlg_set_iconify
				GLOBL	wdlg_set_size
				GLOBL	wdlg_set_tree
				GLOBL	wdlg_set_uniconify


				MODULE	wdlg_close
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,-(a7)
				move.w	#-1,_GemParBlk+aintout
				move.w	#-1,_GemParBlk+aintout+2
				move.l  #$a2000301,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	wdlg_create
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.l	8(a7),_GemParBlk+addrin+12
				move.w	d0,_GemParBlk+aintin
				move.w	d1,_GemParBlk+aintin+2
				clr.l	_GemParBlk+addrout
				move.l  #$a0020004,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	wdlg_delete
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$a3000101,d1
				bra		_aes

				ENDMOD


				MODULE	wdlg_evnt
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l  #$a6000102,d1
				bra		_aes

				ENDMOD


				MODULE	wdlg_get_edit
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,-(a7)
				move.w	#1,_GemParBlk+aintin
				move.w	#-1,_GemParBlk+aintout+2
				move.l  #$a4010201,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	wdlg_get_handle
				
				move.l	a0,_GemParBlk+addrin
				move.w	#3,_GemParBlk+aintin
				move.l  #$a4010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_get_tree
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.w	#0,_GemParBlk+aintin
				move.l  #$a4010103,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_get_udata
				
				move.l	a0,_GemParBlk+addrin
				move.w	#2,_GemParBlk+aintin
				move.l  #$a4010001,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	wdlg_open
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	6(a7),_GemParBlk+addrin+8
				move.w	d0,_GemParBlk+aintin
				move.w	d1,_GemParBlk+aintin+2
				move.w	d2,_GemParBlk+aintin+4
				move.w	4(a7),_GemParBlk+aintin+6
				move.l  #$a1040103,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_redraw
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	d0,_GemParBlk+aintin
				move.w	d1,_GemParBlk+aintin+2
				move.l  #$a7020002,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_set_edit
				
				move.l	a0,_GemParBlk+addrin
				move.w	#0,_GemParBlk+aintin
				move.w	d0,_GemParBlk+aintin+2
				move.l  #$a5020101,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_set_iconify
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.l	8(a7),_GemParBlk+addrin+12
				move.w	#3,_GemParBlk+aintin
				move.w	d0,_GemParBlk+aintin+2
				move.l  #$a5020104,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_set_size
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#2,_GemParBlk+aintin
				move.l  #$a5010102,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_set_tree
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#1,_GemParBlk+aintin
				move.l  #$a5010102,d1
				bra		_aes
				
				ENDMOD


				MODULE	wdlg_set_uniconify
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.l	8(a7),_GemParBlk+addrin+12
				move.w	#4,_GemParBlk+aintin
				move.l  #$a5010104,d1
				bra		_aes
				
				ENDMOD


