				INCLUDE	"gem.i"

				GLOBL	vq_chcells
				GLOBL	v_exit_cur
				GLOBL	v_enter_cur
				GLOBL	v_curup
				GLOBL	v_curdown
				GLOBL	v_curright
				GLOBL	v_curleft
				GLOBL	v_curhome
				GLOBL	v_eeos
				GLOBL	v_eeol
				GLOBL	vs_curaddress
				GLOBL	v_curtext
				GLOBL	v_rvon
				GLOBL	v_rvoff
				GLOBL	vq_curaddress
				GLOBL	vq_tabstatus
				GLOBL	v_hardcopy
				GLOBL	v_dspcur
				GLOBL	v_rmcur
				GLOBL	v_form_adv
				GLOBL	v_output_window
				GLOBL	v_clear_disp_list
				GLOBL	v_bit_image
				GLOBL	vq_scan
				GLOBL	v_alpha_text
				GLOBL	v_orient
				GLOBL	v_copies
				GLOBL	v_trays
				GLOBL	v_tray
				GLOBL	vq_tray_names
				GLOBL	v_page_size
				GLOBL	vq_page_name
				GLOBL	vq_prn_scaling
				GLOBL	vs_palette
				GLOBL	v_sound
				GLOBL	vs_mute
				GLOBL	vs_calibrate
				GLOBL	vq_calibrate
				GLOBL	vt_resolution
				GLOBL	vt_axis
				GLOBL	vt_origin
				GLOBL	vq_tdimensions
				GLOBL	vt_alignment
				GLOBL	vsp_film
				GLOBL	vqp_films
				GLOBL	vqp_filmname
				GLOBL	vqp_state
				GLOBL	vsc_expose
				GLOBL	vsp_state
				GLOBL	vsp_save
				GLOBL	vsp_message
				GLOBL	vqp_error
				GLOBL	v_meta_extents
				GLOBL	v_write_meta
				GLOBL	vm_pagesize
				GLOBL	vm_coords
				GLOBL	vm_filename
				GLOBL	v_bez_qual
				GLOBL	v_offset
				GLOBL	v_fontinit
				GLOBL	v_escape2000
				GLOBL	vq_margins
				GLOBL	vs_document_info
				
; 5,1
				MODULE	vq_chcells

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#1,v_opcode2(a0)
				moveq	#5,d1
				bsr	_VdiCtrl2
				movea.l (a7)+,a1
				move.w	d0,(a1)
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts

				ENDMOD


; 5,2
				MODULE	v_exit_cur

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#2,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,3
				MODULE	v_enter_cur

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#3,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,4
				MODULE	v_curup

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#4,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,5
				MODULE	v_curdown

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#5,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,6
				MODULE	v_curright

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#6,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,7
				MODULE	v_curleft

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#7,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,8
				MODULE	v_curhome

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#8,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,9
				MODULE	v_eeos

			 	lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#9,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,10
				MODULE	v_eeol

			 	lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#10,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,11
				MODULE	vs_curaddress

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	#11,v_opcode2(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,12
				MODULE	v_curtext

				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#12,v_opcode2(a0)
				movea.l (a7)+,a1
				lea	intin(a0),a0
				moveq	#0,d1
				moveq	#0,d2
v_curtext1:		move.b	(a1)+,d1
				beq	v_curtext2
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra	v_curtext1
v_curtext2:		lea	_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,13
				MODULE	v_rvon

			 	lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#13,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,14
				MODULE	v_rvoff

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#14,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,15
				MODULE	vq_curaddress

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#15,v_opcode2(a0)
				moveq	#5,d1
				bsr	_VdiCtrl2
				movea.l (a7)+,a1
				move.w	d0,(a1)
				movea.l (a7)+,a1
				move.w	(a0),(a1)
				rts

				ENDMOD


; 5,16
				MODULE	vq_tabstatus

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#16,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,17
				MODULE	v_hardcopy

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#17,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,18
				MODULE	v_dspcur

				lea	_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#18,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,19
				MODULE	v_rmcur

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#19,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,20
				MODULE	v_form_adv

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#20,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,21
				MODULE	v_output_window

				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#21,v_opcode2(a0)
				movea.l (a7)+,a1
				move.w	(a1)+,ptsin(a0)
				move.w	(a1)+,ptsin+2(a0)
				move.w	(a1)+,ptsin+4(a0)
				move.w	(a1)+,ptsin+6(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,22
				MODULE	v_clear_disp_list

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#22,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,23
				MODULE	v_bit_image

				move.l	a1,-(a7)
				move.l	a0,a1
				lea	_GemParBlk,a0
				lea	intin(a0),a0
				move.w	d1,(a0)+
				move.w	d2,(a0)+
				move.w	8(a7),(a0)+
				move.w	10(a7),(a0)+
				move.w	12(a7),(a0)+
				moveq	#0,d1
				moveq	#5,d2
v_bit_image1:	move.b	(a1)+,d1
				beq	v_bit_image2
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra	v_bit_image1
v_bit_image2:	lea	_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				move.w	d2,v_nintin(a0)
				move.w	#23,v_opcode2(a0)
				movea.l (a7)+,a1
				move.w	(a1)+,ptsin(a0)
				move.w	(a1)+,ptsin+2(a0)
				move.w	(a1)+,ptsin+4(a0)
				move.w	(a1),ptsin+6(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,24
				MODULE	vq_scan

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w  #24,v_opcode2(a0)
				moveq	#5,d1
				bsr		_VdiCtrl2
				move.l	(a7)+,a1
				move.w	d0,(a1)
				move.l	(a7)+,a1
				move.w	(a0)+,(a1)
				move.l	4(a7),a1
				move.w	(a0)+,(a1)
				move.l	8(a7),a1
				move.w	(a0)+,(a1)
				move.l	12(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


; 5,25
				MODULE	v_alpha_text

				movea.l a0,a1
				lea     _GemParBlk,a0
				lea     intin(a0),a0
				clr.w   d1
				clr.w   d2
v_alpha1:		move.b  (a1)+,d1
				beq     v_alpha2
				move.w  d1,(a0)+
				addq.w  #1,d2
				bra     v_alpha1
v_alpha2: 		lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  d2,v_nintin(a0)
				move.w  #25,v_opcode2(a0)
				moveq   #5,d1
				bra     _VdiCtrl2

				ENDMOD


; 5,27
				MODULE	v_orient

			if NEW_MT
				VDIPB 0
				move.w	d1,vdi_intin(sp)
				VDI_JUMP_ESC d0, #5,#27, #0, #1
			else
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w  #1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#27,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2
			endc

				ENDMOD


; 5,28
				MODULE	v_copies

			if NEW_MT
				VDIPB 0
				move.w	d1,vdi_intin(sp)
				VDI_CALL_ESC d0, #5,#28, #0, #1
				move.w	vdi_control+v_nintout(sp),d1
				bne.s	v_copies1
				moveq	#1,d0 ; function not supported by the driver
v_copies1:		VDI_END
				rts
			else
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w  #1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#28,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2
			endc

				ENDMOD


; 5,29
				MODULE	v_trays

			if NEW_MT
				move.l  a0,-(a7)
				move.l  a1,-(a7)
				VDIPB 0
				move.w	d1,vdi_intin(sp)
				move.w	d2,vdi_intin+2(sp)
				VDI_CALL_ESC d0, #5,#29, #0, #2
				move.w	(a0),d2
				cmp.w	#2,d1
				bge		v_trays1
				clr.w	d2
v_trays1:		movea.l (a7)+,a1
				tst.l	a1
				beq.s	v_trays2
				move.w  d2,(a1)
v_trays2:		movea.l (a7)+,a1
				tst.l	a1
				beq.s	v_trays3
				move.w  d0,(a1)
v_trays3:		VDI_END
				rts
			else
				move.l  a0,-(a7)
				move.l  a1,-(a7)
				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #2,v_nintin(a0)
				move.w  #29,v_opcode2(a0)
				move.w  d1,intin(a0)
				move.w  d2,intin+2(a0)
				moveq   #5,d1
				bsr     _VdiCtrl2
				move.w	(a0),d1
				cmp.w	#2,_GemParBlk+v_nintout
				bge		v_trays1
				clr.w	d1
				cmp.w	#1,_GemParBlk+v_nintout
				bge		v_trays1
				clr.w	d0
v_trays1:		movea.l (a7)+,a1
				move.w  d1,(a1)
				movea.l (a7)+,a1
				move.w  d0,(a1)
				rts
			endc

				ENDMOD
				
				MODULE	v_tray

			if NEW_MT
				VDIPB 0
				move.w	d1,vdi_intin(sp)
				VDI_CALL_ESC d0, #5,#29, #0, #1
				rts
			else
				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #1,v_nintin(a0)
				move.w  #29,v_opcode2(a0)
				move.w  d1,intin(a0)
				moveq   #5,d1
				bra     _VdiCtrl2
			endc

				ENDMOD


; 5,36
				MODULE	vq_tray_names

				move.l	a0,d1
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w  #4,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	a1,intin+4(a0)
				move.w	#36,v_opcode2(a0)
				moveq	#5,d1
				bsr	_VdiCtrl2
				move.l	4(a7),a1
				move.w	d0,(a1)
				move.l	8(a7),a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


; 5,37
				MODULE	v_page_size

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w  #1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#37,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,38
				MODULE	vq_page_name

				move.l  a0,d2
				move.l  a1,-(a7)
				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #3,v_nintin(a0)
				move.w  #38,v_opcode2(a0)
				move.w  d1,intin(a0)
				move.l  d2,intin+2(a0)
				moveq   #5,d1
				bsr     _VdiCtrl2
				movea.l (a7)+,a1
				move.l  (a0)+,(a1)
				movea.l 4(a7),a1
				move.l  (a0),(a1)
				rts

				ENDMOD


; 5,39
				MODULE	vq_prn_scaling

				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #2,v_nintin(a0)
				move.w  #39,v_opcode2(a0)
				move.l  #-1,intin(a0)
				moveq   #5,d1
				bsr     _VdiCtrl2
				move.l	-2(a0),d0
				cmp.w	#2,_GemParBlk+v_nintout
				bge		vq_prn_scaling1
				moveq	#-1,d0
vq_prn_scaling1:
				rts

				ENDMOD


; 5,60
				MODULE	vs_palette

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	#60,v_opcode2(a0)
				move.w	d1,intin(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,61
				MODULE	v_sound

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	#61,v_opcode2(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,62
				MODULE	vs_mute

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	#62,v_opcode2(a0)
				move.w	d1,intin(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,76
				MODULE	vs_calibrate

				move.l	a0,d2
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#3,v_nintin(a0)
				move.w	#76,v_opcode2(a0)
				move.w	d1,intin+4(a0)
				move.l	d2,intin(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,77
				MODULE	vq_calibrate

				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	#77,v_opcode2(a0)
				move.w	d1,intin(a0)
				moveq	#5,d1
				bsr	_VdiCtrl2
				move.l	(a7)+,a1
				move.w	d0,(a1)
				move.w	-intout-2+v_nintout(a0),d0
				rts
				
				ENDMOD


; 5,81
				MODULE	vt_resolution

				move.l  a0,-(a7)
				move.l  a1,-(a7)
				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #2,v_nintin(a0)
				move.w  #81,v_opcode2(a0)
				move.w  d1,intin(a0)
				move.w  d2,intin+2(a0)
				moveq   #5,d1
				bsr     _VdiCtrl2
				movea.l (a7)+,a1
				move.w  d0,(a1)
				movea.l (a7)+,a1
				move.w  (a0),(a1)
				rts

				ENDMOD


; 5,82
				MODULE	vt_axis

				move.l  a0,-(a7)
				move.l  a1,-(a7)
				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #2,v_nintin(a0)
				move.w  #82,v_opcode2(a0)
				move.w  d1,intin(a0)
				move.w  d2,intin+2(a0)
				moveq   #5,d1
				bsr     _VdiCtrl2
				movea.l (a7)+,a1
				move.w  d0,(a1)
				movea.l (a7)+,a1
				move.w  (a0),(a1)
				rts

				ENDMOD


; 5,83
				MODULE	vt_origin

				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #2,v_nintin(a0)
				move.w  #83,v_opcode2(a0)
				move.w  d1,intin(a0)
				move.w  d2,intin+2(a0)
				moveq   #5,d1
				bra     _VdiCtrl2

				ENDMOD


; 5,84
				MODULE	vq_tdimensions

				move.l  a0,-(a7)
				move.l  a1,-(a7)
				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				clr.w   v_nintin(a0)
				move.w  #84,v_opcode2(a0)
				moveq   #5,d1
				bsr     _VdiCtrl2
				movea.l (a7)+,a1
				move.w  d0,(a1)
				movea.l (a7)+,a1
				move.w  (a0),(a1)
				rts

				ENDMOD


; 5,85
				MODULE	vt_alignment

				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #2,v_nintin(a0)
				move.w  #85,v_opcode2(a0)
				move.w  d1,intin(a0)
				move.w  d2,intin+2(a0)
				moveq   #5,d1
				bra     _VdiCtrl2

				ENDMOD


; 5,91
				MODULE	vsp_film

				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #2,v_nintin(a0)
				move.w  #91,v_opcode2(a0)
				move.w  d1,intin(a0)
				move.w  d2,intin+2(a0)
				moveq   #5,d1
				bra     _VdiCtrl2

				ENDMOD


				MODULE	vqp_films

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#91,v_opcode2(a0)
				clr.w   intin(a0)
				moveq	#5,d1
				bsr		_VdiCtrl2
				movea.l	(a7)+,a0
				lea		_GemParBlk+intout,a1
				move.w	_GemParBlk+v_nintout,d0
				bra		vqp_film2
vqp_film1:		move.w	(a1)+,d1
				move.b	d1,(a0)+
vqp_film2:		dbf		d0,vqp_film1
				rts

				ENDMOD


; 5,91
				MODULE	vqp_filmname

				move.l	a0,-(a7)
				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #1,v_nintin(a0)
				move.w  #91,v_opcode2(a0)
				move.w  d1,intin(a0)
				moveq   #5,d1
				bsr     _VdiCtrl2
				subq.l	#2,a0
				move.w  control+v_nintout,d1
				move.l	(a7)+,a1
vqp_loop:		move.w	(a0)+,d0
				move.b	d0,(a1)+
				subq.w	#1,d1
				bne		vqp_loop
				move.w  control+v_nintout,d0
				rts
				
				ENDMOD


; 5,92
				MODULE	vqp_state

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#92,v_opcode2(a0)
				moveq	#5,d1
				bsr		_VdiCtrl2
				movea.l	(a7)+,a1
				move.w	d0,(a1)
				movea.l	(a7)+,a1
				move.w	(a0)+,(a1)
				movea.l	4(a7),a1
				move.w	(a0)+,(a1)
				movea.l	8(a7),a1
				move.w	(a0)+,(a1)
				movea.l	12(a7),a1
				move.w	(a0)+,(a1)
				movea.l	16(a7),a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


; 5,93
				MODULE	vsc_expose

				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
				move.w  #1,v_nintin(a0)
				move.w  #93,v_opcode2(a0)
				move.w  d1,intin(a0)
				moveq   #5,d1
				bra     _VdiCtrl2

				ENDMOD

; 5,93
				MODULE	vsp_state

				lea	_GemParBlk,a1
				clr.w	v_nptsin(a1)
				move.w	#21,v_nintin(a1)
				move.w	#93,v_opcode2(a1)
				lea	intin(a1),a1
				move.w	d1,(a1)+			; port
				move.w	d2,(a1)+			; film_num
				move.w	8(a7),(a1)+			; lightness
				move.w	10(a7),(a1)+		; interlace
				move.w	12(a7),(a1)+		; planes
				moveq	#8-1,d1
vsp_state1:		move.l	(a0)+,(a1)+			; indexes
				dbf	d1,vsp_state1
				lea	_GemParBlk,a0
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,94
				MODULE	vsp_save

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#94,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,95
				MODULE	vsp_message

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#95,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD

; 5,96
				MODULE	vqp_error

				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#96,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,98
				MODULE	v_meta_extents

				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#98,v_opcode2(a0)
				move.w	d1,ptsin(a0)
				move.w	d2,ptsin+2(a0)
				move.w	4(a7),ptsin+4(a0)
				move.w	6(a7),ptsin+6(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,99
				MODULE	v_write_meta

				move.l	a0,_VdiParBlk+v_intin
				move.l	a1,_VdiParBlk+v_ptsin
				lea	_GemParBlk,a0
				move.w	d1,v_nintin(a0)
				move.w	d2,v_nptsin(a0)
				move.w	#99,v_opcode2(a0)
				moveq	#5,d1
				bsr	_VdiCtrl2
				move.l	#_GemParBlk+intin,_VdiParBlk+v_intin
				move.l	#_GemParBlk+ptsin,_VdiParBlk+v_ptsin
				rts

				ENDMOD


; 5,99,0
				MODULE	vm_pagesize

			 	lea     _GemParBlk,a0
				clr.w   intin(a0)
				move.w  d1,intin+2(a0)
				move.w  d2,intin+4(a0)
				clr.w   v_nptsin(a0)
				move.w  #3,v_nintin(a0)
				move.w  #99,v_opcode2(a0)
				moveq   #5,d1
				bra     _VdiCtrl2

				ENDMOD


; 5,99,1
				MODULE	vm_coords

				lea     _GemParBlk,a0
				move.w  #1,intin(a0)
				move.w  d1,intin+2(a0)
				move.w  d2,intin+4(a0)
				move.w  4(a7),intin+6(a0)
				move.w  6(a7),intin+8(a0)
				clr.w   v_nptsin(a0)
				moveq   #5,d1
				move.w  d1,v_nintin(a0)
				move.w  #99,v_opcode2(a0)
				bra     _VdiCtrl2

				ENDMOD


; 5,99,32,1
				MODULE	v_bez_qual

				move.l	a0,-(a7)
				lea     _GemParBlk,a0
				move.w  #32,intin(a0)
				move.w  #1,intin+2(a0)
				move.w  d1,intin+4(a0)
				clr.w   v_nptsin(a0)
				move.w  #3,v_nintin(a0)
				moveq   #5,d1
				move.w  #99,v_opcode2(a0)
				bsr     _VdiCtrl2
				move.l	(a7)+,a0
				move.w	d0,(a0)
				rts
				
				ENDMOD


; 5,100
				MODULE	vm_filename

				move.l	a0,-(a7)
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#100,v_opcode2(a0)
				lea	intin(a0),a0
				movea.l (a7)+,a1
				moveq	#0,d1
				moveq	#0,d2
vm_filename1:	move.b	(a1)+,d1
				beq	vm_filename2
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra	vm_filename1
vm_filename2:	lea	_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,101
				MODULE	v_offset

				lea	_GemParBlk,a0
				move.w	d1,intin(a0)
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	#101,v_opcode2(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,102
				MODULE	v_fontinit

				move.l	a0,a1
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	#102,v_opcode2(a0)
				move.l	a1,intin(a0)
				moveq	#5,d1
				bra	_VdiCtrl2

				ENDMOD


; 5,2000
				MODULE	v_escape2000
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	#2000,v_opcode2(a0)
				move.w	d1,intin(a0)
				moveq	#5,d1
				bra		_VdiCtrl2
				
				ENDMOD
				

				MODULE	vq_margins

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#2100,v_opcode2(a0)
				moveq	#5,d1
				bsr		_VdiCtrl2
				cmp.w	#7,_GemParBlk+v_nintout
				bge		vq_margins1
				clr.w	d0
vq_margins1:	movea.l	(a7)+,a1
				move.w	(a0)+,(a1)
				movea.l	(a7)+,a1
				move.w	(a0)+,(a1)
				movea.l	4(a7),a1
				move.w	(a0)+,(a1)
				movea.l	8(a7),a1
				move.w	(a0)+,(a1)
				movea.l	12(a7),a1
				move.w	(a0)+,(a1)
				movea.l	16(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


; 5,2103
				MODULE	vs_document_info
				
				tst.w	d1
				bne		vs_doc3 ; 16-bit not supported
				move.l	a0,a1
				lea	_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2103,v_opcode2(a0)
				lea	intin(a0),a0
				move.w	d0,(a0)+
				moveq	#0,d1
				moveq	#1,d2
vs_doc1:		move.b	(a1)+,d1
				beq	vs_doc2
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra	vs_doc1
vs_doc2:		lea	_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#5,d1
				bsr	_VdiCtrl2
				cmp.w	#1,_GemParBlk+v_nintout
				bge		vs_doc4
vs_doc3:		moveq	#0,d0
vs_doc4:		rts
				
				ENDMOD
