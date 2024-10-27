				INCLUDE	"gem.i"

				GLOBL	scrp_read,scrp_write,scrp_clear
				
				
				MODULE	scrp_read
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$50000101,d1
				bra		_aes

				ENDMOD


				MODULE	scrp_write
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$51000101,d1
				bra		_aes

				ENDMOD


				MODULE	scrp_clear
				
				move.l  #$52000100,d1
				bra		_aes

				ENDMOD

