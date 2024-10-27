				INCLUDE	"gem.i"

				GLOBL	appl_init
				GLOBL	appl_read
				GLOBL	appl_write
				GLOBL	appl_find
				GLOBL	appl_tplay
				GLOBL	appl_trecord
				GLOBL	appl_bvset
				GLOBL	appl_yield
				GLOBL	_appl_yield
				GLOBL	appl_exit
				GLOBL	appl_search
				GLOBL	appl_getinfo
				GLOBL	appl_getinfo_str
				GLOBL	appl_control
				
				GLOBL	gl_ap_version
				GLOBL	gl_apid
;				GLOBL	gl_ap_count



				MODULE	appl_init
				
				move.l	#$0a000100,d1
				moveq	#-1,d0
				move.w	d0,_GemParBlk+aintout
				move.w	d0,_GemParBlk+global+4
				bsr		_aes
				move.w	_GemParBlk+global,gl_ap_version
;				move.w	_GemParBlk+global+2,gl_ap_count
				move.w	d0,gl_apid
				rts
				
				BSS
gl_ap_version:	ds.w	1
;gl_ap_count:	ds.w	1
                DATA
gl_apid:		dc.w	-1
				TEXT

				ENDMOD


				MODULE	appl_read
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$0b020101,d1
				bra		_aes

				ENDMOD


				MODULE	appl_write
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l	#$0c020101,d1
				bra		_aes

				ENDMOD


				MODULE	appl_find
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$0d000101,d1
				bra		_aes

				ENDMOD

				
				MODULE	appl_tplay

				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l	#$0e020101,d1
				bra		_aes

				ENDMOD


				MODULE	appl_trecord
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$0f010101,d1
				bra		_aes
				
				ENDMOD
				

				MODULE	appl_bvset
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l	#$10020100,d1
				bra		_aes

				ENDMOD


				MODULE	appl_yield
				
				move.l	#$11000100,d1
				bra		_aes
				
				ENDMOD

				MODULE	_appl_yield
				
				move.l  a2,-(a7)
				move.w	#$00c9,d0
				trap	#2
				move.l  (a7)+,a2
				rts

				ENDMOD

				MODULE	appl_search
				
				move.l	a1,-(a7)
				move.w	d0,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l	#$12010301,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD
				
				MODULE	appl_exit
				
				move.w  #-1,gl_apid
				move.l  #$13000100,d1
				bra		_aes

				ENDMOD


				MODULE	appl_getinfo
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				move.w	d0,_GemParBlk+aintin
				; clear aintout array for buggy programs
				; that dont check the return code
				clr.l _GemParBlk+aintout
				clr.l _GemParBlk+aintout+4
				clr.l _GemParBlk+aintout+8
				move.l  #$82010500,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0)+,(a1)
				move.l	8(a7),a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	appl_getinfo_str
				
				move.w	d0,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	4(a7),_GemParBlk+addrin+8
				move.l	8(a7),_GemParBlk+addrin+12
				clr.l _GemParBlk+aintout
				clr.l _GemParBlk+aintout+4
				clr.l _GemParBlk+aintout+8
				move.l  #$82010104,d1
				bra		_aes
				
				ENDMOD


				MODULE	appl_control
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l	#$81020101,d1
				bra		_aes
				
				ENDMOD
