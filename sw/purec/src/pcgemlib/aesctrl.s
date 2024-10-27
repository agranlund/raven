				GLOBL	_aes
				GLOBL	_aes1
				GLOBL	aes
				GLOBL	_crystal
				GLOBL	_AesParBlk
				GLOBL	aespb
				GLOBL	vq_aes
				
				INCLUDE	"gem.i"
				XREF	appl_init
				

				MODULE	_aes
				moveq	#0,d0
_aes1:			lea.l	_GemParBlk+acontrl,a1
				clr.l	(a1)+
				clr.l	(a1)+
				move.w	d0,(a1)		/* contrl[4] = naddrout */
				movep.l	d1,-7(a1)
				move.w	#$00c8,d0
				move.l	a2,-(a7)
				move.l	#aespb,d1
				trap	#2
				lea.l	_GemParBlk+aintout,a0
				move.w	(a0)+,d0
				move.l	(a7)+,a2
				rts

				DATA
_AesParBlk:
aespb:			dc.l	_GemParBlk+acontrl
				dc.l	_GemParBlk+global
				dc.l	_GemParBlk+aintin
				dc.l	_GemParBlk+aintout
				dc.l	_GemParBlk+addrin
				dc.l	_GemParBlk+addrout
				TEXT

				ENDMOD
				
; short aes(AESPB *)
				MODULE	aes
				move.l	a2,-(a7)
				move.l	a0,-(a7)
				move.w	#$00c8,d0
				move.l	a0,d1
				trap	#2
				move.l	(a7)+,a0
				move.l	a_intout(a0),a0
				move.w	(a0)+,d0
				move.l	(a7)+,a2
				rts
				ENDMOD
				

				IFNE	0
;
; this table is no longer used;
; crystal(opcode) just doesnt work anymore because
; there are some ambigious AES calls that
; have the same opcode but different
; values for nintin/ninout/naddrin/naddrout
;
aestab:
				dc.b	0,1,0,0 ; appl_init
				dc.b	2,1,1,0 ; appl_read
				dc.b	2,1,1,0 ; appl_write
				dc.b	0,1,1,0 ; appl_find
				dc.b	2,1,1,0 ; appl_tplay
				dc.b	1,1,1,0 ; appl_trecord
				dc.b	2,1,0,0 ; appl_bvset
				dc.b	0,1,0,0 ; appl_yield
				dc.b	1,3,1,0 ; appl_search
				dc.b	0,1,0,0 ; appl_exit
				
				dc.b	0,1,0,0 ; evnt_keybd
				dc.b	3,5,0,0 ; evnt_button
				dc.b	5,5,0,0 ; evnt_mouse
				dc.b	0,1,1,0 ; evnt_mesag
				dc.b	2,1,0,0 ; evnt_timer
				dc.b	16,7,1,0 ; evnt_multi
				dc.b	2,1,0,0 ; evnt_dclick
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				
				dc.b	1,1,1,0 ; menu_bar
				dc.b	2,1,1,0 ; menu_icheck
				dc.b	2,1,1,0 ; menu_ienable
				dc.b	2,1,1,0 ; menu_tnormal
				dc.b	1,1,2,0 ; menu_text
				dc.b	1,1,1,0 ; menu_register
				dc.b	1,1,0,0 ; menu_unregister,menu_popup=2,1,2,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	2,1,1,0 ; objc_add
				dc.b	1,1,1,0 ; objc_delete
				dc.b	6,1,1,0 ; objc_draw
				dc.b	4,1,1,0 ; objc_find
				dc.b	1,3,1,0 ; objc_offset
				dc.b	2,1,1,0 ; objc_order
				dc.b	4,2,1,0 ; objc_edit
				dc.b	8,1,1,0 ; objc_change
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	1,1,1,0 ; form_do
				dc.b	9,1,1,0 ; form_dial
				dc.b	1,1,1,0 ; form_alert
				dc.b	1,1,0,0 ; form_error
				dc.b	0,5,1,0 ; form_center
				dc.b	3,3,1,0 ; form_keybd
				dc.b	2,2,1,0 ; form_button
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	4,3,0,0 ; graf_rubberbox
				dc.b	8,3,0,0 ; graf_dragbox
				dc.b	6,1,0,0 ; graf_movebox
				dc.b	8,1,0,0 ; graf_growbox
				dc.b	8,1,0,0 ; graf_shrinkbox
				dc.b	4,1,1,0 ; graf_watchbox
				dc.b	3,1,1,0 ; graf_slidebox
				dc.b	0,5,0,0 ; graf_handle
				dc.b	1,1,1,0 ; graf_mouse
				dc.b	0,5,0,0 ; graf_mkstate
				dc.b	0,1,1,0 ; scrp_read
				dc.b	0,1,1,0 ; scrp_write
				dc.b	0,0,0,0 ; scrp_clear
				dc.b	0,0,0,0 
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,2,2,0 ; fsel_input
				dc.b	0,2,3,0 ; fsel_exinput
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	5,1,0,0 ; wind_create
				dc.b	5,1,0,0 ; wind_open
				dc.b	1,1,0,0 ; wind_close
				dc.b	1,1,0,0 ; wind_delete
				dc.b	2,5,0,0 ; wind_get
				dc.b	6,1,0,0 ; wind_set
				dc.b	2,1,0,0 ; wind_find
				dc.b	1,1,0,0 ; wind_update
				dc.b	6,5,0,0 ; wind_calc
				dc.b	0,0,0,0 ; wind_new
				dc.b	0,1,1,0 ; rsrc_load
				dc.b	0,1,0,0 ; rsrc_free
				dc.b	2,1,0,1 ; rsrc_gaddr
				dc.b	2,1,1,0 ; rsrc_saddr
				dc.b	1,1,1,0 ; rsrc_obfix
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	0,1,2,0 ; shel_read
				dc.b	3,1,2,0 ; shel_write
				dc.b	1,1,1,0 ; shel_get
				dc.b	1,1,1,0 ; shel_put
				dc.b	0,1,1,0 ; shel_find
				dc.b	0,1,3,0 ; shel_envrn
				dc.b	0,0,0,0 ; shel_rdef
				dc.b	0,0,0,0 ; shel_wdef
				dc.b	0,0,0,0
				dc.b	0,0,0,0
				dc.b	6,6,0,0 ; xgrf_stepcalc
				dc.b	9,1,0,0 ; xgrf_2box
				
				ENDC
				


				MODULE	vq_aes
				clr.w	_GemParBlk+global ; clr gl_ap_version
				bsr		appl_init
				move.w	_GemParBlk+global+4,d0 ; fetch gl_apid
				tst.w	_GemParBlk+global ; check gl_ap_version
				bne		vq_aes1
				moveq.l	#-1,d0
vq_aes1:		rts
				ENDMOD
				

; void _crystal(AESPB *)
				MODULE	_crystal
				move.l	a2,-(a7)
				move.l	a0,d1
				move.w	#$00c8,d0
				trap	#2
				move.l	(a7)+,a2
				rts
				ENDMOD
