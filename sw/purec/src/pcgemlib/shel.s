				INCLUDE	"gem.i"
				
				GLOBL	shel_read,shel_write,shel_get,shel_put,shel_find
				GLOBL	shel_environ,shel_envrn,shel_rdef,shel_wdef
				GLOBL	shel_help
				
				
				MODULE	shel_read
				
				movem.l	a0-a1,_GemParBlk+addrin
				move.l	#$78000102,d1
				bra		_aes

				ENDMOD


				MODULE	shel_write
				
				movem.l	a0-a1,_GemParBlk+addrin
				movem.w	d0-d2,_GemParBlk+aintin
				move.l  #$79030102,d1
				bra		_aes

				ENDMOD


				MODULE	shel_get
				
				move.w	d0,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l  #$7a010101,d1
				bra		_aes

				ENDMOD


				MODULE	shel_put
				
				move.w	d0,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l  #$7b010101,d1
				bra		_aes

				ENDMOD


				MODULE	shel_find
				
				move.l	a0,_GemParBlk+addrin
				move.l	#$7c000101,d1
				bra		_aes

				ENDMOD


				MODULE	shel_environ
				
shel_envrn:		movem.l	a0-a1,_GemParBlk+addrin
				move.l  #$7d000102,d1
				bra		_aes

				ENDMOD


				MODULE	shel_rdef
				
				movem.l	a0-a1,_GemParBlk+addrin
				move.l  #$7e000102,d1
				bra		_aes

				ENDMOD


				MODULE	shel_wdef
				
				movem.l	a0-a1,_GemParBlk+addrin
				move.l  #$7f000002,d1
				bra		_aes

				ENDMOD



				MODULE	shel_help
				
				move.w	d0,_GemParBlk+aintin
				movem.l	a0-a1,_GemParBlk+addrin
				move.l  #$80010102,d1
				bra		_aes

				ENDMOD
