				INCLUDE "gem.i"

				GLOBL	vswr_mode
				GLOBL	vs_color
				GLOBL	vsf_color
				GLOBL	vsf_interior
				GLOBL	vsf_perimeter
				GLOBL	vsf_xperimeter
				GLOBL	vsf_style
				GLOBL	vsf_udpat
				GLOBL	vsl_color
				GLOBL	vsl_ends
				GLOBL	vsl_type
				GLOBL	vsl_udsty
				GLOBL	vsl_width
				GLOBL	vsm_color
				GLOBL	vsm_height
				GLOBL	vsm_type
				GLOBL	vst_alignment
				GLOBL	vst_color
				GLOBL	vst_effects
				GLOBL	vst_font
				GLOBL	vst_height
				GLOBL	vst_point
				GLOBL	vst_rotation
				GLOBL	vst_fg_color
				GLOBL	vsf_fg_color
				GLOBL	vsl_fg_color
				GLOBL	vsm_fg_color
				GLOBL	vsr_fg_color
				GLOBL	vst_bg_color
				GLOBL	vsf_bg_color
				GLOBL	vsl_bg_color
				GLOBL	vsm_bg_color
				GLOBL	vsr_bg_color
				GLOBL	vqt_fg_color
				GLOBL	vqf_fg_color
				GLOBL	vql_fg_color
				GLOBL	vqm_fg_color
				GLOBL	vqr_fg_color
				GLOBL	vqt_bg_color
				GLOBL	vqf_bg_color
				GLOBL	vql_bg_color
				GLOBL	vqm_bg_color
				GLOBL	vqr_bg_color
				GLOBL	vs_ctab
				GLOBL	vs_ctab_entry
				GLOBL	vs_dflt_ctab
				GLOBL	vs_hilite_color
				GLOBL	vs_min_color
				GLOBL	vs_max_color
				GLOBL	vs_weight_color
				GLOBL	vst_name
				GLOBL	vst_width
				GLOBL	vst_charmap
				GLOBL	vst_map_mode
				GLOBL	vst_track_offset
				GLOBL	vst_kern_info
				GLOBL	vst_kern

				
				MODULE	vswr_mode

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#32,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vs_color

				lea		_GemParBlk,a1
				clr.w	v_nptsin(a1)
				move.w	#4,v_nintin(a1)
				lea		intin(a1),a1
				move.w	d1,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0),(a1)
				lea		_GemParBlk,a0
				moveq	#14,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_color

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#25,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_interior

				lea		_GemParBlk,a0
				move.w	d1,intin(a0)
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				moveq	#23,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_perimeter

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#104,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_xperimeter

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				moveq	#104,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_style

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#24,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_udpat

				lea		_GemParBlk,a1
				clr.w	v_nptsin(a1)
				lsl.w	#4,d1
				move.w	d1,v_nintin(a1)
				subq.w	#1,d1
				lea		intin(a1),a1
vsf_udpat1:		move.w	(a0)+,(a1)+
				dbf		d1,vsf_udpat1
				lea		_GemParBlk,a0
				moveq	#112,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsl_color

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#17,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsl_ends

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				moveq	#108,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsl_type

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#15,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsl_udsty

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#113,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsl_width

				lea		_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	d1,ptsin(a0)
				clr.w	ptsin+2(a0)
				moveq	#16,d1
				bsr		_VdiCtrl
				move.w	_GemParBlk+ptsout,d0
				rts

				ENDMOD


				MODULE	vsm_color

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#20,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsm_height

				lea		_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				clr.w	v_nintin(a0)
				clr.w	ptsin(a0)
				move.w	d1,ptsin+2(a0)
				moveq	#19,d1
				bsr		_VdiCtrl
				move.w	_GemParBlk+ptsout+2,d0
				rts

				ENDMOD


				MODULE	vsm_type

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#18,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_alignment

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				moveq	#39,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+intout,a0
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	vst_color

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#22,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_effects

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#106,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_font

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#21,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_height

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				clr.w	v_nintin(a0)
				clr.w	ptsin(a0)
				move.w	d1,ptsin+2(a0)
				moveq	#12,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
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


				MODULE	vst_point

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				clr.w	ptsin(a0)
				move.w	d1,intin(a0)
				moveq	#107,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
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


				MODULE	vst_rotation

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#13,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_fg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#200,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_fg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#200,d1
				move.w	#1,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vsl_fg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#200,d1
				move.w	#2,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vsm_fg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#200,d1
				move.w	#3,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vsr_fg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#200,d1
				move.w	#4,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vst_bg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#201,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vsf_bg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#201,d1
				move.w	#1,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vsl_bg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#201,d1
				move.w	#2,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vsm_bg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#201,d1
				move.w	#3,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vsr_bg_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#201,d1
				move.w	#4,v_opcode2(a0)
				bra		_VdiCtrl2

				ENDMOD


				MODULE	vqt_fg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#202,d1
				bsr		_VdiCtrl
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqf_fg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#202,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vql_fg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#202,d1
				move.w	#2,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqm_fg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#202,d1
				move.w	#3,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqr_fg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#202,d1
				move.w	#4,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqt_bg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#203,d1
				bsr		_VdiCtrl
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqf_bg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#203,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vql_bg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#203,d1
				move.w	#2,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqm_bg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#203,d1
				move.w	#3,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqr_bg_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#203,d1
				move.w	#4,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq.w	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vs_ctab
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.l	4(a1),d1
				lsr.w	#1,d1
				move.w	d1,v_nintin(a0)
				move.l	a1,_VdiParBlk+v_intin
				move.w	#205,d1
				bsr		_VdiCtrl
				move.l	#_GemParBlk+intin,_VdiParBlk+v_intin
				rts
				
				ENDMOD


				MODULE	vs_ctab_entry
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#7,v_nintin(a0)
				lea		intin(a0),a0
				move.w	d1,(a0)+
				move.l	d2,(a0)+
				move.l	(a1)+,(a0)+
				move.l	(a1),(a0)
				move.w	#205,d1
				move.w	#1,v_opcode2(a0)
				bra		_VdiCtrl2
				
				ENDMOD


				MODULE	vs_dflt_ctab
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#205,d1
				move.w	#2,v_opcode2(a0)
				bra		_VdiCtrl2
				
				ENDMOD


				MODULE	vs_hilite_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#207,d1
				bsr		_VdiCtrl
				cmp.w	#1,_GemParBlk+v_nintout
				bge		vs_hil1
				moveq	#0,d0
vs_hil1:		rts

				ENDMOD


				MODULE	vs_min_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#207,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				cmp.w	#1,_GemParBlk+v_nintout
				bge		vs_min1
				moveq	#0,d0
vs_min1:		rts

				ENDMOD


				MODULE	vs_max_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#207,d1
				move.w	#2,v_opcode2(a0)
				bsr		_VdiCtrl2
				cmp.w	#1,_GemParBlk+v_nintout
				bge		vs_max1
				moveq	#0,d0
vs_max1:		rts

				ENDMOD


				MODULE	vs_weight_color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#207,d1
				move.w	#3,v_opcode2(a0)
				bsr		_VdiCtrl2
				cmp.w	#1,_GemParBlk+v_nintout
				bge		vs_weight1
				moveq	#0,d0
vs_weight1:		rts

				ENDMOD


				MODULE	vst_name

				move.l	a1,-(a7)
				lea		_GemParBlk,a1
				move.w	#1,v_nptsin(a1)
				lea		intin(a1),a1
				move.w	d1,(a1)+
				moveq	#0,d1
				moveq	#0,d2
vst_name1:		move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		vst_name1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#230,d1
				bsr		_VdiCtrl
				move.l	(a7)+,a1
				move.w	_GemParBlk+v_nintout,d2
				subq.w	#1,d2
				ble		vst_name4
				bra		vst_name3
vst_name2:		move.w	(a0)+,d1
				move.b	d1,(a1)+
vst_name3:		dbf		d2,vst_name2
vst_name4:		clr.b	(a1)
				rts
				
				ENDMOD


				MODULE	vst_width

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				move.w	#1,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	d1,ptsin(a0)
				clr.w	ptsin+2(a0)
				move.w	#231,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
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


				MODULE	vst_charmap
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#236,d1
				bra		_VdiCtrl
				
				ENDMOD


				MODULE	vst_map_mode
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#1,intin+2(a0)
				move.w	#-1,intout(a0)
				move.w	#236,d1
				bra		_VdiCtrl
				
				ENDMOD


				MODULE	vst_kern
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				move.w	#237,d1
				bsr		_VdiCtrl
				move.l	(a7)+,a1
				move.w	d0,(a1)
				move.l	(a7)+,a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vst_track_offset
				
vst_kern_info: ; another name
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#4,v_nintin(a0)
				move.w	#255,intin(a0)
				move.w	d2,intin+2(a0)
				move.l	d1,intin+4(a0)
				move.w	#237,d1
				bsr		_VdiCtrl
				move.l	(a7)+,a1
				move.w	d0,(a1)
				move.l	(a7)+,a1
				move.w	(a0),(a1)
				rts
				
				ENDMOD


