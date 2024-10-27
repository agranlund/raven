				INCLUDE	"gem.i"
				
				GLOBL	vro_cpyfm
				GLOBL	vrt_cpyfm
				GLOBL	v_get_pixel
				GLOBL	vr_trnfm
				GLOBL	vr_transfer_bits
				
				
				MODULE	vro_cpyfm

				move.l	a0,d2
				lea		_GemParBlk,a0 
				move.w	#4,v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.l	a1,v_param(a0) 
				move.l	4(a7),v_param+4(a0) 
				move.w	d1,intin(a0)
				move.l	d2,a1
				lea		ptsin(a0),a0 
				move.l	(a1)+,(a0)+ 
				move.l	(a1)+,(a0)+ 
				move.l	(a1)+,(a0)+ 
				move.l	(a1),(a0) 
				lea		_GemParBlk,a0 
				moveq	#109,d1 
				bra		_VdiCtrl
		
				ENDMOD
		
		
				MODULE	vrt_cpyfm
		
				move.l	a0,d2
				lea		_GemParBlk,a0 
				move.w	#4,v_nptsin(a0)
				move.w	#3,v_nintin(a0)
				move.l	a1,v_param(a0) 
				move.l	4(a7),v_param+4(a0) 
				move.w	d1,intin(a0)
				movea.l 8(a7),a1 
				move.l	(a1),intin+2(a0)
				move.l	d2,a1
				lea		ptsin(a0),a0 
				move.l	(a1)+,(a0)+ 
				move.l	(a1)+,(a0)+ 
				move.l	(a1)+,(a0)+ 
				move.l	(a1),(a0) 
				lea		_GemParBlk,a0 
				moveq	#121,d1 
				bra		_VdiCtrl
		
				ENDMOD
		
		
				MODULE	v_get_pixel
		
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				moveq	#105,d1
				bsr		_VdiCtrl
				movea.l (a7)+,a1
				move.w	d0,(a1)
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts
		
				ENDMOD
		
		
				MODULE	vr_trnfm
					        
				move.l	a0,d1
				lea		_GemParBlk,a0 
				clr.w	v_nptsin(a0) 
				clr.w	v_nintin(a0) 
				move.l	d1,v_param(a0)
				move.l	a1,v_param+4(a0)
				moveq	#110,d1 
				bra		_VdiCtrl
		
				ENDMOD
		
		
				MODULE	vr_transfer_bits
		
				move.l	a0,d2
				lea		_GemParBlk,a0 
				move.w	#4,v_nptsin(a0)
				move.w	#4,v_nintin(a0)
				move.l	d2,v_param(a0) 
				move.l	a1,v_param+4(a0) 
				clr.l	v_param+8(a0) 
				move.w	d1,intin(a0)
				clr.l	intin+2(a0) 
				clr.w	intin+6(a0) 
				lea		ptsin(a0),a0 
				movea.l 4(a7),a1 
				move.l	(a1)+,(a0)+
				move.l	(a1),(a0)+
				movea.l 8(a7),a1 
				move.l	(a1)+,(a0)+ 
				move.l	(a1),(a0) 
				lea		_GemParBlk,a0 
				move.w	#170,d1 
				bra		_VdiCtrl
		
				ENDMOD
