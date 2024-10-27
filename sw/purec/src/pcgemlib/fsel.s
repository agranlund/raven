				INCLUDE	"gem.i"

				GLOBL	fsel_input,fsel_exinput,fsel_boxinput
				
				
				MODULE	fsel_input
				
				movem.l	a0-a1,_GemParBlk+addrin
				move.l  #$5a000202,d1
				bsr		_aes
				movea.l 4(a7),a1
				move.w	(a0),(a1)
				rts

				ENDMOD
				
				
				MODULE	fsel_exinput
				
				movem.l	a0-a1,_GemParBlk+addrin
				move.l	8(a7),_GemParBlk+addrin+8
				move.l  #$5b000203,d1
				bsr		_aes
				movea.l 4(a7),a1
				move.w	(a0),(a1)
				rts
				
				MODULE	fsel_boxinput
				
				movem.l	a0-a1,_GemParBlk+addrin
				move.l	8(a7),_GemParBlk+addrin+8
				move.l	12(a7),_GemParBlk+addrin+12
				move.l  #$5b000204,d1

				move.l	a2,-(a7)
				lea.l	_GemParBlk+acontrl,a1
				clr.l	(a1)+
				clr.l	(a1)+
				clr.w	(a1)
				movep.l	d1,-7(a1)
				moveq	#80-1,d0
				lea		_GemParBlk+global,a0
				lea		my_global,a1
loop:			move.w	(a0)+,(a1)+
				dbf		d0,loop
				clr.w	my_global+4
				
				move.w	#$00c8,d0
				move.l	#my_aespb,d1
				trap	#2
				lea.l	_GemParBlk+aintout,a0
				move.w	(a0)+,d0
				move.l	(a7)+,a2

				movea.l 4(a7),a1
				move.w	(a0),(a1)
				rts
				
my_aespb:		dc.l	_GemParBlk+acontrl
				dc.l	my_global
				dc.l	_GemParBlk+aintin
				dc.l	_GemParBlk+aintout
				dc.l	_GemParBlk+addrin
				dc.l	_GemParBlk+addrout

my_global:		ds.w	80

				ENDMOD


