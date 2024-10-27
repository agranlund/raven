				INCLUDE	"gem.i"

				GLOBL	menu_bar,menu_icheck,menu_ienable,menu_tnormal
				GLOBL	menu_text,menu_register
				GLOBL   menu_unregister,menu_popup,menu_attach
				GLOBL   menu_istart,menu_settings
				GLOBL	menu_click
				
				
				MODULE	menu_bar
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$1e010101,d1
				bra		_aes

				ENDMOD


				MODULE	menu_icheck
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$1f020101,d1
				bra		_aes

				ENDMOD


				MODULE	menu_ienable
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$20020101,d1
				bra		_aes

				ENDMOD


				MODULE	menu_tnormal
				
				move.l	a0,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$21020101,d1
				bra		_aes

				ENDMOD


				MODULE	menu_text
				
				movem.l	a0-a1,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$22010102,d1
				bra		_aes

				ENDMOD


				MODULE	menu_register
				
				move.w	d0,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l  #$23010101,d1
				bra		_aes

				ENDMOD

				MODULE	menu_unregister
				
				move.w	d0,_GemParBlk+aintin
				move.l  #$24010100,d1
				bra		_aes

				ENDMOD

				MODULE	menu_popup
				
				movem.l a0-a1,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$24020102,d1
				bra		_aes

				ENDMOD

				MODULE	menu_attach
				
				movem.l a0-a1,_GemParBlk+addrin
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$25020102,d1
				bra		_aes

				ENDMOD

				MODULE	menu_istart
				
				move.l  a0,_GemParBlk+addrin
				movem.w	d0-d2,_GemParBlk+aintin
				move.l  #$26030101,d1
				bra		_aes

				ENDMOD

				MODULE	menu_settings
				
				move.l  a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$27010101,d1
				bra		_aes

				ENDMOD

				MODULE	menu_click
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$25020100,d1
				bra		_aes

				ENDMOD

