				INCLUDE	"gem.i"
				
				GLOBL	EvntMulti
				GLOBL	evnt_multi_fast

NINTIN			equ 16				
NINTOUT			equ	7

				DATA

contrl:			dc.w	25		; opcode
				dc.w	NINTIN	; nintin
				dc.w	NINTOUT	; nintout
				dc.w	1		; naddrin
				dc.w	0		; naddrout

my_aespb:		dc.l	contrl
				dc.l	_GemParBlk+global
				dc.l	0
				dc.l	0
				dc.l	_GemParBlk+addrin
				dc.l	_GemParBlk+addrout
				
				TEXT
				
				MODULE	EvntMulti

				move.l	a2,-(a7)
				move.l	a0,my_aespb+8			; aintin
				adda.w	#NINTIN*2,a0
				move.l	a0,-(a7)
				move.l	a0,my_aespb+12			; aintout
				adda.w	#NINTOUT*2,a0
				move.l	a0,_GemParBlk+addrin
				move.l	#$c8,d0
				move.l	#my_aespb,d1
				trap	#2
				move.l	(a7)+,a0
				move.w	(a0),d0
				movea.l	(a7)+,a2
				rts
				
				ENDMOD
				
				MODULE	evnt_multi_fast
				
				move.l	a1,_GemParBlk+addrin
				move.l	a0,my_aespb+8			; aintin
				move.l	4(a7),my_aespb+12		; aintout
				move.l	a2,-(a7)
				move.l	#$c8,d0
				move.l	#my_aespb,d1
				trap	#2
				movea.l	(a7)+,a2
				move.l	4(a7),a0
				move.w	(a0)+,d0
				rts
				
				ENDMOD
				
