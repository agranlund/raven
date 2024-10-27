				INCLUDE	"gem.i"

				GLOBL	rsrc_load,rsrc_free,rsrc_gaddr,rsrc_saddr,rsrc_obfix
				GLOBL	rsrc_rcfix
				
				MODULE	rsrc_load
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$6e000101,d1
				bra		_aes

				ENDMOD


				MODULE	rsrc_free
				
				move.l	#$6f000100,d1
				bra		_aes

				ENDMOD


				MODULE	rsrc_gaddr
				
				move.l	a0,-(a7)
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$70020100,d1
				moveq	#1,d0
				bsr		_aes1
				movea.l (a7)+,a1
				move.l	_GemParBlk+addrout,(a1)
				rts

				ENDMOD


				MODULE	rsrc_saddr
				
				move.l	a0,_GemParBlk+addrin
				movem.w d0-d1,_GemParBlk+aintin
				move.l  #$71020101,d1
				bra		_aes

				ENDMOD


				MODULE	rsrc_obfix
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$72010101,d1
				bra		_aes

				ENDMOD


				MODULE	rsrc_rcfix
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$73000101,d1
				bra		_aes

				ENDMOD



