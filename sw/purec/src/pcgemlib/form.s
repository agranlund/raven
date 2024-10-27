				INCLUDE	"gem.i"
				
				GLOBL	form_do
				GLOBL	form_dial
				GLOBL	form_dial_grect
				GLOBL	form_alert
				GLOBL	form_error
				GLOBL	form_center
				GLOBL	form_keybd
				GLOBL	form_button
				
				GLOBL	form_popup
				GLOBL	form_xdial
				GLOBL	form_xdial_grect
				GLOBL	form_xdo
				GLOBL	form_xerr
				GLOBL	form_wbutton
				GLOBL	form_wkeybd
				GLOBL	form_center_grect
				GLOBL	xfrm_popup
				
				MODULE	form_do
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$32010101,d1
				bra		_aes
				
				ENDMOD


				MODULE	form_dial
				
				lea     _GemParBlk+aintin,a0
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	4(a7),(a0)+
				move.l	8(a7),(a0)+
				move.l	12(a7),(a0)
				move.l  #$33090100,d1
				bra		_aes
				
				ENDMOD


				MODULE	form_dial_grect
				
				move.l	a2,d1
				lea     _GemParBlk+aintin,a2
				move.w	d0,(a2)+
				move.l  a0,d0
				bne.s   fmd_dial1
				move.l	d0,(a2)+
				move.l	d0,(a2)+
				bra.s   fmd_dial2
fmd_dial1:
				move.l	(a0)+,(a2)+
				move.l	(a0),(a2)+
fmd_dial2:
				move.l	(a1)+,(a2)+
				move.l	(a1),(a2)+
				move.l	d1,a2
				move.l  #$33090100,d1
				bra		_aes
				
				ENDMOD


				MODULE	form_alert
				
				move.l	a0,_GemParBlk+addrin
				move.w	d0,_GemParBlk+aintin
				move.l  #$34010101,d1
				bra		_aes

				ENDMOD


				MODULE	form_error
				
				move.w	d0,_GemParBlk+aintin
				move.l  #$35010100,d1
				bra		_aes
				
				ENDMOD


				MODULE	form_center
				
				move.l	a1,-(a7)
				move.l	a0,_GemParBlk+addrin
				move.l  #$36000501,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				movea.l 8(a7),a1
				move.w	(a0)+,(a1)
				movea.l 12(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	form_center_grect
				
				move.l	a1,-(a7)
				move.l	a0,_GemParBlk+addrin
				move.l  #$36000501,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	form_keybd

				move.l	a1,-(a7)
				lea.l	_GemParBlk,a1
				move.w	d0,aintin(a1)
				move.w	d2,aintin+2(a1)
				move.w	d1,aintin+4(a1)
				move.l	a0,addrin(a1)
				move.l  #$37030301,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				rts
				
				ENDMOD


				MODULE	form_wkeybd

				move.l	a1,-(a7)
				lea.l	_GemParBlk,a1
				move.w	d0,aintin(a1)
				move.w	d2,aintin+2(a1)
				move.w	d1,aintin+4(a1)
				move.w	12(a7),aintin+6(a1)
				move.l	a0,addrin(a1)
				move.l  #$40040301,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l 4(a7),a1
				move.w	(a0)+,(a1)
				rts
				
				ENDMOD


				MODULE	form_button
				
				move.l	a1,-(a7)
				movem.w	d0-d1,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l  #$38020201,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	form_wbutton
				
				move.l	a1,-(a7)
				lea		_GemParBlk,a1
				movem.w	d0-d2,aintin(a1)
				move.l	a0,addrin(a1)
				move.l  #$3f030201,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	form_popup
				
				movem.w	d0-d1,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l  #$87020101,d1
				bra		_aes

				ENDMOD


				MODULE	form_xdial
				
				move.l	a0,_GemParBlk+addrin
				lea     _GemParBlk+aintin,a0
				clr.l   addrin+4-aintin(a0)
				move.w	d0,(a0)+
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.l	4(a7),(a0)+
				move.l	8(a7),(a0)+
				move.l	12(a7),(a0)
				move.l  #$33090102,d1
				bra		_aes
				
				ENDMOD


				MODULE	form_xdial_grect
				
				move.l	a1,d1
				lea     _GemParBlk+aintin,a1
				move.l	4(a7),addrin-aintin(a1)
				clr.l   addrin+4-aintin(a1)
				move.w	d0,(a1)+
				move.l  a0,d0
				bne.s   fmd_xdial1
				move.l	d0,(a1)+
				move.l	d0,(a1)+
				bra.s   fmd_xdial2
fmd_xdial1:
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)+
fmd_xdial2:
				move.l	d1,a0
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)+
				move.l  #$33090102,d1
				bra		_aes
				
				ENDMOD


				MODULE	form_xdo
				
				move.l	a1,-(a7)
				move.l	a0,_GemParBlk+addrin
				move.l	8(a7),_GemParBlk+addrin+4
				move.l	12(a7),_GemParBlk+addrin+8
				move.w	d0,_GemParBlk+aintin
				move.l  #$32010203,d1
				bsr		_aes
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				rts
				
				ENDMOD


				MODULE	form_xerr
				
				move.l	d0,_GemParBlk+aintin
				move.l	a0,_GemParBlk+addrin
				move.l  #$88020101,d1
				bra		_aes

				ENDMOD


				MODULE	xfrm_popup
				
				move.l	a0,_GemParBlk+addrin        /* tree */
				move.l	a1,_GemParBlk+addrin+4      /* init */
				move.l	8(a7),_GemParBlk+addrin+8   /* param */
				lea     _GemParBlk+aintin,a1
				move.w	d0,(a1)+                    /* intin[0] = x */
				move.w	d1,(a1)+                    /* intin[1] = y */
				move.w	d2,(a1)+                    /* intin[2] = firstscrlob */
				move.l	4(a7),(a1)+                 /* intin[3/4] = lastscrlob/nlines */
				move.l	12(a7),a0
				move.w	(a0),(a1)                   /* intin[5] = *lastscrlpos */
				move.l  #$87060203,d1
				bsr		_aes
				move.l	12(a7),a1                   /* *lastscrlpos = intout[1] */
				move.w	(a0),(a1)
				rts
				
				ENDMOD
