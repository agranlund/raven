				INCLUDE	"gem.i"

				GLOBL	xgrf_stepcalc
				GLOBL	xgrf_2box
				GLOBL	xgrf_rbox
				
				
				MODULE	xgrf_stepcalc
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.l	12(a7),(a1)+
				move.w	16(a7),(a1)
				move.l	#$82060600,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	10(a7),a1
				move.w	(a0)+,(a1)
				move.l	14(a7),a1
				move.w	(a0)+,(a1)
				move.l	18(a7),a1
				move.w	(a0)+,(a1)
				rts
				
				ENDMOD


				MODULE	xgrf_2box
				
				lea		_GemParBlk+aintin,a1
				move.w	d0,(a1)+
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.l	4(a7),(a1)+
				move.l	8(a7),(a1)+
				move.l	12(a7),(a1)
				move.l	#$83090100,d1
				bra		_aes
				
				ENDMOD
				
				

				MODULE	xgrf_rbox
				
				move.l	a1,d0
				lea		_GemParBlk+aintin,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)+
				move.l	d0,a0
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)+
				move.l	#$84080000,d1
				bra		_aes
				
				ENDMOD
