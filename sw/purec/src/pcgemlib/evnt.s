				INCLUDE	"gem.i"

				GLOBL	evnt_keybd,evnt_button,evnt_mouse,evnt_mesag
				GLOBL	evnt_dclick
				GLOBL	evnt_timer
				GLOBL	evnt_multi
				
				
				MODULE	evnt_keybd
				
				move.l  #$14000100,d1
				bra		_aes
				
				ENDMOD


				MODULE	evnt_button
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				movem.w	d0-d2,_GemParBlk+aintin
				move.l  #$15030500,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				movea.l 8(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	evnt_mouse
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea.l	_GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.w	12(a7),(a0)+
				move.w	14(a7),(a0)+
				move.l  #$16050500,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 8(a7),a1
				move.w	(a0)+,(a1)
				movea.l 12(a7),a1
				move.w	(a0)+,(a1)
				rts
				
				ENDMOD


				MODULE	evnt_mesag
				
				move.l	a0,_GemParBlk+addrin
				move.l  #$17000101,d1
				bra		_aes
				
				ENDMOD


	ifne 0
				GLOBL	evnt_timer

				MODULE	evnt_timer
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$18020100,d1
				bra		_aes
				
				ENDMOD
	endc


				MODULE	evnt_timer
				
				swap	d0
				move.l	d0,_GemParBlk+aintin
				move.l  #$18020100,d1
				bra		_aes
				
				ENDMOD


	ifne 0
				GLOBL	evnt_multi

				MODULE	evnt_multi
			
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1) ; ev_mmgpbuff
				move.w	d0,(a1)+            ; ev_mflags
				move.w	d1,(a1)+            ; ev_mbclick
				move.w	d2,(a1)+            ; ev_mbmask
				move.l	8(a7),(a1)+         ; ev_mbstate + ev_mm1flags
				move.l	12(a7),(a1)+        ; ev_mm1x + ev_mm1y
				move.l	16(a7),(a1)+        ; ev_mm1width + ev_mm1height
				move.l	20(a7),(a1)+        ; ev_mm1flags + ev_mm2x
				move.l	24(a7),(a1)+        ; ev_mm2y + ev_mm2width
				move.l	28(a7),(a1)+        ; ev_mm2height + ev_mtlocount
				move.w	32(a7),(a1)         ; ev_mthicount
				move.l  #$19100701,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				move.l  a2,d1
				lea     30(a7),a2
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				move.l  d1,a2
				rts

				ENDMOD
	endc


				MODULE	evnt_multi
			
				move.l	a1,-(a7)
				lea.l	_GemParBlk+aintin,a1
				move.l	a0,addrin-aintin(a1) ; ev_mmgpbuff
				move.w	d0,(a1)+            ; ev_mflags
				move.w	d1,(a1)+            ; ev_mbclick
				move.w	d2,(a1)+            ; ev_mbmask
				move.l	8(a7),(a1)+         ; ev_mbstate + ev_mm1flags
				move.l	12(a7),(a1)+        ; ev_mm1x + ev_mm1y
				move.l	16(a7),(a1)+        ; ev_mm1width + ev_mm1height
				move.l	20(a7),(a1)+        ; ev_mm1flags + ev_mm2x
				move.l	24(a7),(a1)+        ; ev_mm2y + ev_mm2width
				move.w	28(a7),(a1)+        ; ev_mm2height
				move.l	30(a7),d0           ; 
				swap	d0
				move.l	d0,(a1)             ; ev_interval
				move.l  #$19100701,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				move.l  a2,d1
				lea     30(a7),a2
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				movea.l (a2)+,a1
				move.w	(a0)+,(a1)
				move.l  d1,a2
				rts

				ENDMOD


				MODULE	evnt_dclick
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l  #$1a020100,d1
				bra		_aes

				ENDMOD
				
				
