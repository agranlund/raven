				INCLUDE	"gem.i"

				GLOBL	pdlg_add_printers
				GLOBL	pdlg_add_sub_dialogs
				GLOBL	pdlg_close
				GLOBL	pdlg_create
				GLOBL	pdlg_delete
				GLOBL	pdlg_dflt_settings
				GLOBL	pdlg_do
				GLOBL	pdlg_evnt
				GLOBL	pdlg_free_settings
				GLOBL	pdlg_get_setsize
				GLOBL	pdlg_new_settings
				GLOBL	pdlg_open
				GLOBL	pdlg_remove_printers
				GLOBL	pdlg_remove_sub_dialogs
				GLOBL	pdlg_save_default_settings
				GLOBL	pdlg_update
				GLOBL	pdlg_use_settings
				GLOBL	pdlg_validate_settings
				
				
				MODULE	pdlg_close
				
				move.l	a1,-(a7)
				move.l	a0,_GemParBlk+addrin
				moveq #-1,d0
				move.l  d0,_GemParBlk+aintout+2
				move.l	#$cb000301,d1
				bsr		_aes
				move.w	(a0)+,d2
				move.l	(a7)+,d1
				beq	pdlg_c1
				move.l	d1,a1
				move.w	d2,(a1)
pdlg_c1:
				move.l	4(a7),d1
				beq	pdlg_c2
				move.l	d1,a1
				move.w	(a0),(a1)
pdlg_c2:
				rts
				
				ENDMOD
				
				
				MODULE	pdlg_create
				
				move.w	d0,_GemParBlk+aintin
				move.l	#$c8010000,d1
				moveq	#1,d0
				clr.l	_GemParBlk+addrout
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	pdlg_delete
				
				move.l	a0,_GemParBlk+addrin
				clr.w	_GemParBlk+aintout
				move.l	#$c9000101,d1
				bra		_aes
				
				ENDMOD


				MODULE	pdlg_do
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.w	d0,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cf010103,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_evnt
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				clr.l	_GemParBlk+aintout
				move.l	#$ce000203,d1
				bsr		_aes
				move.l	8(a7),a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD
				
				
				MODULE	pdlg_get_setsize
				
				move.w	#0,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cc010200,d1
				bsr		_aes
				move.l	-2(a0),d0
				rts
				
				ENDMOD
				
				
				MODULE	pdlg_open
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.w	d0,_GemParBlk+aintin
				move.w	d1,_GemParBlk+aintin+2
				move.w	d2,_GemParBlk+aintin+4
				clr.w	_GemParBlk+aintout
				move.l	#$ca030103,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_add_printers
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				clr.w	_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010102,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_remove_printers
				
				move.l	a0,_GemParBlk+addrin
				move.w	#1,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010101,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_update
				
				move.l	a0,_GemParBlk+addrin
				clr.l	_GemParBlk+addrin+4
				move.l	a1,_GemParBlk+addrin+8
				move.w	#2,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010103,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_add_sub_dialogs
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#3,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010102,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_remove_sub_dialogs
				
				move.l	a0,_GemParBlk+addrin
				move.w	#4,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010101,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_new_settings
				
				move.l	a0,_GemParBlk+addrin
				move.w	#5,_GemParBlk+aintin
				clr.l	_GemParBlk+addrout
				move.l	#$cd010001,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD
				
				
				MODULE	pdlg_free_settings
				
				move.l	a0,_GemParBlk+addrin
				move.w	#6,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010101,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_dflt_settings
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#7,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010102,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_validate_settings
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#8,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010102,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_use_settings
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#9,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010102,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	pdlg_save_default_settings
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.w	#10,_GemParBlk+aintin
				clr.w	_GemParBlk+aintout
				move.l	#$cd010102,d1
				bra		_aes
				
				ENDMOD
				
				
