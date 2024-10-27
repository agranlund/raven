				INCLUDE	"gem.i"

				GLOBL	fslx_close
				GLOBL	fslx_do
				GLOBL	fslx_evnt
				GLOBL	fslx_getnxtfile
				GLOBL	fslx_open
				GLOBL	fslx_set_flags
				
				
				MODULE	fslx_close
				
				move.l	a0,_GemParBlk+addrin
				move.l	#$bf000101,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	fslx_do
				
				move.l	a1,-(a7)
				lea		_GemParBlk+addrin,a1
				move.l	a0,(a1)+			; title
				move.l	(a7)+,a0
				move.l	a0,(a1)+			; path
				move.l	4(a7),(a1)+			; fname
				move.l	8(a7),(a1)+			; patterns
				move.l	12(a7),(a1)+		; filter
				move.l	16(a7),(a1)+		; paths
				lea		_GemParBlk+aintin,a1
				move.w	d0,(a1)+			; pathlen
				move.w	d1,(a1)+			; fnamelen
				move.l	20(a7),a0
				move.w	(a0),(a1)+			; sort_mode
				move.w	d2,(a1)+			; flags
				move.l	#$c2040406,d1
				moveq	#2,d0
				bsr		_aes1
				move.l	24(a7),a1			; button
				move.w	(a0)+,(a1)
				move.l	28(a7),a1			; nfiles
				move.w	(a0)+,(a1)
				move.l	20(a7),a1			; sort_mode
				move.w	(a0)+,(a1)
				move.l	_GemParBlk+addrout+4,a0
				move.l	32(a7),a1			; pattern
				move.l	a0,(a1)
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	fslx_evnt
				
				move.l	a0,-(a7)
				lea		_GemParBlk+addrin,a0
				move.l	(a7)+,(a0)+			; fsd
				move.l	a1,(a0)+			; events
				move.l	4(a7),(a0)+			; path
				move.l	8(a7),(a0)+			; fname
				move.l	#$c1000404,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	12(a7),a1			; button
				move.w	(a0)+,(a1)
				move.l	16(a7),a1			; nfiles
				move.w	(a0)+,(a1)
				move.l	20(a7),a1			; sort_mode
				move.w	(a0)+,(a1)
				move.l	24(a7),a1
				move.l	_GemParBlk+addrout,(a1)
				rts
				
				ENDMOD


				MODULE	fslx_getnxtfile
				
				move.l	a0,_GemParBlk+addrin
				move.l	a1,_GemParBlk+addrin+4
				move.l	#$c0000102,d1
				bra		_aes
				
				ENDMOD
				
				
				MODULE	fslx_open
				
				move.l	a1,-(a7)
				lea		_GemParBlk+aintin,a1
				move.w	d0,(a1)+			; x
				move.w	d1,(a1)+			; y
				move.w	d2,(a1)+			; pathlen
				move.w	16(a7),(a1)+		; fnamelen
				move.w	30(a7),(a1)+		; sort_mode
				move.w	32(a7),(a1)+		; flags
				lea		_GemParBlk+addrin,a1
				move.l	a0,(a1)+			; title
				move.l	8(a7),(a1)+			; path
				move.l	12(a7),(a1)+		; fname
				move.l	18(a7),(a1)+		; patterns
				move.l	22(a7),(a1)+		; filter
				move.l	26(a7),(a1)+		; paths
				clr.l	_GemParBlk+addrout
				move.l	#$be060106,d1
				moveq	#1,d0
				bsr		_aes1
				move.l	(a7)+,a0
				move.w	d0,(a0)
				move.l	_GemParBlk+addrout,a0
				rts
				
				ENDMOD


				MODULE	fslx_set_flags
				
				move.l	a0,-(a7)
				lea		_GemParBlk+aintin,a1
				clr.w	(a0)+
				move.w	d0,(a0)+
				move.l	#$c3020200,d1
				bsr		_aes
				move.l	(a7)+,a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD
