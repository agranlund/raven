				INCLUDE	"gem.i"

				GLOBL	vqf_attributes
				GLOBL	vqin_mode
				GLOBL	vql_attributes
				GLOBL	vqm_attributes
				GLOBL	vqt_attributes
				GLOBL	vqt_extent
				GLOBL	vqt_extentn
				GLOBL	vqt_extent16
				GLOBL	vqt_extent16n
				GLOBL	vqt_fontinfo
				GLOBL	vqt_name
				GLOBL	vqt_width
				GLOBL	vq_cellarray
				GLOBL	vq_color
				GLOBL	vq_extnd
				GLOBL	vqt_devinfo
				GLOBL	vq_scrninfo
				GLOBL	vqt_char_index
				GLOBL	v_color2value
				GLOBL	v_value2color
				GLOBL	v_color2nearest
				GLOBL	vq_px_format
				GLOBL	vq_ctab
				GLOBL	vq_ctab_entry
				GLOBL	vq_ctab_id
				GLOBL	v_ctab_idx2vdi
				GLOBL	v_ctab_vdi2idx
				GLOBL	v_ctab_idx2value
				GLOBL	v_get_ctab_id
				GLOBL	vq_dflt_ctab
				GLOBL	v_create_ctab
				GLOBL	v_delete_ctab
				GLOBL	v_create_itab
				GLOBL	v_delete_itab
				GLOBL	vq_hilite_color
				GLOBL	vq_min_color
				GLOBL	vq_max_color
				GLOBL	vq_weight_color
				GLOBL	vqt_xfntinfo
				GLOBL	vqt_name_and_id
				GLOBL	vqt_fontheader
				GLOBL	vqt_trackkern
				GLOBL	vqt_pairkern
				GLOBL	v_getbitmap_info
				GLOBL	vqt_get_table
				GLOBL	vqt_cachesize


				MODULE	vqf_attributes

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#37,d1
				bsr		_VdiCtrl
				movea.l	(a7)+,a1
				move.w	d0,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	vqin_mode

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.l	d1,intin(a0)
				move.w	#115,d1
				bsr		_VdiCtrl
				movea.l	(a7)+,a0
				move.w	d0,(a0)
				rts

				ENDMOD


				MODULE	vql_attributes

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				clr.l	intout+6(a0) /* clr intout[3/4]; not always returned */
				move.w	#35,d1
				bsr		_VdiCtrl
				movea.l	(a7)+,a1
				move.w	d0,(a1)+     /* attrib[0] = intout[0] = linetype */
				move.w	(a0)+,(a1)+  /* attrib[1] = intout[1] = linecolor */
				move.w	(a0)+,(a1)+  /* attrib[2] = intout[2] = writing mode */
				move.w	ptsout-intout-6(a0),(a1)+ /* attrib[3] = ptsout[0] = line width */
				moveq	#0,d1
				moveq	#0,d2
				move.w	v_nintout-intout-6(a0),d0
				subq.w	#3,d0
				ble		vql_a1
				move.w	(a0)+,d1
				move.w	(a0)+,d2
vql_a1:
				move.w	d1,(a1)+  /* attrib[4] = intout[3] = line start */
				move.w	d2,(a1)+  /* attrib[5] = intout[4] = line end */
				rts

				ENDMOD


				MODULE	vqm_attributes

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#36,d1
				bsr		_VdiCtrl
				movea.l	(a7)+,a1
				move.w	d0,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	ptsout+2-intout-8(a0),(a1)+
				rts

				ENDMOD


				MODULE	vqt_attributes

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#38,d1
				bsr		_VdiCtrl
				movea.l	(a7)+,a1
				move.w	d0,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				lea		ptsout-intout-12(a0),a0
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	vqt_extent

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				lea		intin(a0),a0
				moveq	#0,d1
				moveq	#0,d2
vqt_extent1:	move.b	(a1)+,d1
				beq		vqt_extent2
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra		vqt_extent1
vqt_extent2:	lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#116,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	vqt_extent16

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				lea		intin(a0),a0
				moveq	#0,d1
				moveq	#0,d2
vqt_extent161:	move.w	(a1)+,d1
				beq		vqt_extent162
				move.w	d1,(a0)+
				addq.w	#1,d2
				bra		vqt_extent161
vqt_extent162:	lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				moveq	#116,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	vqt_extentn

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	d1,v_nintin(a0)
				lea		intin(a0),a0
				moveq	#0,d2
				tst.w	d1
				beq		vqt_extentn2
vqt_extentn1:	move.b	(a1)+,d2
				move.w	d2,(a0)+
				subq.w	#1,d1
				bne.s	vqt_extentn1
vqt_extentn2:	clr.w	(a0)
				lea		_GemParBlk,a0
				moveq	#116,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	vqt_extent16n

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	d1,v_nintin(a0)
				lea		intin(a0),a0
				moveq	#0,d2
				tst.w	d1
				beq		vqt_extent16n2
vqt_extent16n1:	move.w	(a1)+,d2
				move.w	d2,(a0)+
				subq.w	#1,d1
				bne.s	vqt_extent16n1
vqt_extent16n2:	clr.w	(a0)
				lea		_GemParBlk,a0
				moveq	#116,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts

				ENDMOD


				MODULE	vqt_fontinfo

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#131,d1
				bsr		_VdiCtrl
				movea.l	(a7)+,a1
				move.w	d0,(a1)+ ; minADE = intout(0)
				movea.l	(a7)+,a1
				move.w	(a0),(a1) ; maxADE = intout(1)
				lea		_GemParBlk,a0
				movea.l	4(a7),a1
				move.w	ptsout+2(a0),(a1)+ ; distances[0] = ptsout[1] (bottom)
				move.w	ptsout+6(a0),(a1)+ ; distances[1] = ptsout[3] (descent)
				move.w	ptsout+10(a0),(a1)+ ; distances[2] = ptsout[5] (half)
				move.w	ptsout+14(a0),(a1)+ ; distances[3] = ptsout[7] (ascent)
				move.w	ptsout+18(a0),(a1) ; distances[4] = ptsout[9] (top)
				movea.l	8(a7),a1
				move.w	ptsout(a0),(a1) ; maxwidth = ptsout[0]
				movea.l	12(a7),a1
				move.w	ptsout+4(a0),(a1)+ ; effects[0] = ptsout[2]
				move.w	ptsout+8(a0),(a1)+ ; effects[1] = ptsout[4]
				move.w	ptsout+12(a0),(a1)+ ; effects[2] = ptsout[6]
				rts

				ENDMOD


				MODULE	vqt_name

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				clr.w	v_opcode2(a0)
				clr.w	intout+33*2(a0)	; clear fsm flag
				move.w	#130,d1
				bsr		_VdiCtrl2
				movea.l	(a7)+,a1
				moveq	#33-1,d1
vqt_name1:		move.w	(a0)+,d2
				move.b	d2,(a1)+
				dbf		d1,vqt_name1
				clr.b	(a1)
				rts

				ENDMOD


				MODULE	vqt_width

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#117,d1
				bsr		_VdiCtrl
				lea		_GemParBlk+ptsout,a0
				movea.l	(a7)+,a1
				move.w	(a0)+,(a1)
				addq.w	#2,a0
				movea.l	(a7)+,a1
				move.w	(a0)+,(a1)
				addq.w	#2,a0
				movea.l	4(a7),a1
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	vq_cellarray

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				move.w	#2,v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	d1,v_param(a0)
				move.w	d2,v_param+2(a0)
				movea.l	(a7)+,a1
				lea		ptsin(a0),a0
				move.w	(a1)+,(a0)+
				move.w	(a1)+,(a0)+
				move.w	(a1)+,(a0)+
				move.w	(a1),(a0)
				muls	d1,d2
				move.w	d2,-(a7)
				move.w	#27,d1
				bsr		_VdiCtrl
				move.w	(a7)+,d1
				subq.w	#2,d1
				movea.l	16(a7),a1
				move.w	d0,(a1)+
vq_cellarray1:	move.l	(a0)+,(a1)+
				dbf		d1,vq_cellarray1
				lea		_GemParBlk+v_param+4,a0
				movea.l	(a7)+,a1
				move.w	(a0)+,(a1)
				movea.l	4(a7),a1
				move.w	(a0)+,(a1)
				movea.l	8(a7),a1
				move.w	(a0)+,(a1)
				rts

				ENDMOD


				MODULE	vq_color

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				move.w	#26,d1
				bsr		_VdiCtrl
				movea.l (a7)+,a1
				move.w	(a0)+,(a1)+
				move.w	(a0)+,(a1)+
				move.w	(a0),(a1)
				rts

				ENDMOD


				MODULE	vq_extnd

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#102,d1
				bsr		_VdiCtrl
				movea.l	(a7)+,a1
				move.w	d0,(a1)+
				moveq	#45-1-1,d1
vq_extnd1:		move.w	(a0)+,(a1)+
				dbf		d1,vq_extnd1
				lea		_GemParBlk+ptsout,a0
				moveq	#12-1,d1
vq_extnd2:		move.w	(a0)+,(a1)+
				dbf		d1,vq_extnd2
				rts

				ENDMOD


				MODULE	vq_scrninfo

; beware: vq_scrninfo returns 272 words, but our
; ptsout is only 256 words long
; this will overwrite 16 words beyond the array
; (first 4 entries of AES addrin array in this case)

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	#1,v_opcode2(a0)
				move.w	#2,intin(a0)
				clr.w   v_nintout(a0)
				move.w	#102,d1
				bsr		_VdiCtrl2
				movea.l	(a7)+,a1
				move.w	d0,(a1)+
				move.w  -intout-2+v_nintout(a0),d0
				move.w	#272-1-1,d1
vq_scrninfo1:	move.w	(a0)+,(a1)+
				dbf		d1,vq_scrninfo1
				rts

				ENDMOD


; long vq_vgdos(void)
				GLOBL	vq_vgdos
				MODULE	vq_vgdos
				move.l	a2,-(a7)
				moveq	#-2,d0
				trap	#2
				move.l	(a7)+,a2
				/* MiNT may trash the upper bits of d0 */
				cmp.w	#-2,d0
				bne vq_vgdos1
				moveq.l	#-2,d0
vq_vgdos1:
				rts
				ENDMOD


; short vq_gdos(void)
				GLOBL	vq_gdos
				MODULE	vq_gdos

				bsr		vq_vgdos
				cmp.w	#-2,d0
				sne		d0
				ext.w	d0
				rts

				ENDMOD


; void  vqt_devinfo( _WORD handle, _WORD devnum, _WORD *devexits, char *filename, char *device_name );
; _WORD vq_devinfo (_WORD handle, _WORD device, _WORD *dev_exists, char *file_name, char *device_name);
				MODULE	vqt_devinfo
				pea		(a1)
				pea		(a0)
				lea		_GemParBlk,a0
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				clr.w	v_nptsin(a0)
				move.w	#248,d1
				bsr		_VdiCtrl
				move.l	(a7)+,a1
				subq.l	#2,a0
				move.w	ptsout-intout(a0),d0 ; devexists = first entry in ptsout
				move.w	d0,(a1)
				bne		vqt_dev1
				move.l	(a7)+,a1
				clr.b	(a1)
				move.l	4(a7),a1
				clr.b	(a1)
				rts
vqt_dev1:		move.l	(a7)+,a1
				move.w	v_nintout-intout(a0),d1
				bra		vqt_dev3
vqt_dev2:		move.w	(a0)+,d2
				move.b	d2,(a1)+
vqt_dev3:		dbf		d1,vqt_dev2
				
				move.l	4(a7),a1
				move.w	v_nptsout-intout(a0),d1
				move.w	v_nptsin-intout(a0),d0
				lea		ptsout-intout+2(a0),a0
				subq.w	#1,d1
				add.w	d1,d1
				bne		vqt_dev5
				tst.w	d0
				bne		vqt_dev5
				move.w	d0,d1
				add.w	d1,d1
				bra		vqt_dev5
vqt_dev4:		move.b	(a0)+,d2
				move.b	d2,(a1)+
vqt_dev5:		dbf		d1,vqt_dev4
				rts
				
				ENDMOD
				

				MODULE	vqt_char_index
		
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintout(a0)
				move.w	#3,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				move.w	4(a7),intin+4(a0)
				move.w	#190,d1
				bsr		_VdiCtrl
				cmp.w	#1,_GemParBlk+v_nintout
				bge		vqt_char1
				moveq	#-1,d0
vqt_char1:
				rts
				
				ENDMOD
		
		
				MODULE	v_color2value
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#204,d1
				bsr		_VdiCtrl
				move.l	-2(a0),d0
				rts
				
				ENDMOD


		
				MODULE	v_value2color
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#204,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	-2(a0),d0
				rts
				
				ENDMOD


				MODULE	v_color2nearest
				
				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	(a1)+,intin+4(a0)
				move.l	(a1),intin+8(a0)
				move.w	#204,d1
				move.w	#2,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vq_px_format
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#204,d1
				move.w	#3,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD

		
				MODULE	vq_ctab
				
				move.l	a0,_VdiParBlk+v_intout
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	d1,intin(a0)
				move.w	#206,d1
				bsr		_VdiCtrl
				move.l	#_GemParBlk+intout,_VdiParBlk+v_intout
				move.w	_GemParBlk+v_nintout,d0
				beq		vq_ctab1
				moveq	#1,d0
vq_ctab1:		rts
				
				ENDMOD


				MODULE	vq_ctab_entry
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#206,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vq_ctab_id
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#206,d1
				move.w	#2,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	-2(a0),d0
				rts
				
				ENDMOD

		
				MODULE	v_ctab_idx2vdi
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#206,d1
				move.w	#3,v_opcode2(a0)
				bra		_VdiCtrl2
				
				ENDMOD


				MODULE	v_ctab_vdi2idx
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#206,d1
				move.w	#4,v_opcode2(a0)
				bra		_VdiCtrl2
				
				ENDMOD

		
				MODULE	v_ctab_idx2value
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#206,d1
				move.w	#5,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	-2(a0),d0
				rts
				
				ENDMOD


				MODULE	v_get_ctab_id
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#206,d1
				move.w	#6,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	-2(a0),d0
				rts
				
				ENDMOD


				MODULE	vq_dflt_ctab
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	a1,_VdiParBlk+v_intout
				move.w	#206,d1
				move.w	#7,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	#_GemParBlk+intout,_VdiParBlk+v_intout
				move.w	_GemParBlk+v_nintout,d0
				beq		vq_dflt_ctab1
				moveq	#1,d0
vq_dflt_ctab1:	rts
				
				ENDMOD


				MODULE	v_create_ctab
				
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#4,v_nintin(a0)
				move.l	d1,intin(a0)
				move.l	d2,intin+4(a0)
				move.w	#206,d1
				move.w	#8,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	-2(a0),d0
				cmp.w	#2,_GemParBlk+v_nintout
				bge		v_create_ctab1
				moveq	#0,d0
v_create_ctab1:	move.l	d0,a0
				rts
				
				ENDMOD


				MODULE	v_delete_ctab
				
				move.l	a0,d1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	d1,intin(a0)
				move.w	#206,d1
				move.w	#9,v_opcode2(a0)
				bra		_VdiCtrl2
				
				ENDMOD


				MODULE	v_create_itab
				
				move.l	a0,d2
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#3,v_nintin(a0)
				move.l	d2,intin(a0)
				move.w	d1,intin+4(a0)
				clr.l	intin+6(a0)
				move.w	#208,d1
				bsr		_VdiCtrl
				move.l	-2(a0),d0
				cmp.w	#2,_GemParBlk+v_nintout
				bge		v_create_itab1
				moveq	#0,d0
v_create_itab1:	move.l	d0,a0
				rts
				
				ENDMOD


				MODULE	v_delete_itab
				
				move.l	a0,d2
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#3,v_nintin(a0)
				move.l	d2,intin(a0)
				clr.w	intin+4(a0)
				move.w	#208,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				cmp.w	#1,_GemParBlk+v_nintout
				bge		v_delete_itab1
				moveq	#0,d0
v_delete_itab1:
				rts
				
				ENDMOD


				MODULE	vq_hilite_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#209,d1
				bsr		_VdiCtrl
				subq	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vq_min_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#209,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vq_max_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#209,d1
				move.w	#2,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vq_weight_color
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#209,d1
				move.w	#3,v_opcode2(a0)
				bsr		_VdiCtrl2
				subq	#2,a0
				move.l	(a0)+,d0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqt_xfntinfo
				
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#5,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				move.w	4(a7),intin+4(a0)
				move.l	a1,intin+6(a0)
				move.w	#229,d1
				bsr		_VdiCtrl
				move.w	(a0),d0 ; note: returns intout[1]
				rts
				
				ENDMOD


				MODULE	vqt_name_and_id

				move.l	a1,-(a7)
				lea		_GemParBlk,a1
				move.w	#1,v_nptsin(a1)
				lea		intin(a1),a1
				move.w	d1,(a1)+
				moveq	#0,d1
				moveq	#0,d2
vqt_nameid1:	move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		vqt_nameid1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#230,d1
				move.w	#100,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	(a7)+,a1
				move.w	_GemParBlk+v_nintout,d2
				subq.w	#1,d2
				ble		vqt_nameid4
				bra		vqt_nameid3
vqt_nameid2:	move.w	(a0)+,d1
				move.b	d1,(a1)+
vqt_nameid3:	dbf		d2,vqt_nameid2
vqt_nameid4:	clr.b	(a1)
				rts
				
				ENDMOD


				MODULE	vqt_fontheader
				
				move.l	a1,-(a7)
				move.l	a0,d1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	d1,intin(a0)
				move.w	#232,d1
				bsr		_VdiCtrl
				move.l	(a7)+,a1
				move.w	_GemParBlk+v_nintout,d0
				beq		vqt_fontheader2
vqt_fontheader1:move.w	(a0)+,d2
				move.b	d2,(a1)+
				beq		vqt_fontheader2
				subq.w	#1,d0
				bne		vqt_fontheader1
vqt_fontheader2:clr.b	(a1)
				rts
				
				ENDMOD


				MODULE	vqt_trackkern
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#234,d1
				bsr		_VdiCtrl
				subq.l	#2,a0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)
				move.l	(a7)+,a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqt_pairkern
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				move.w	#235,d1
				bsr		_VdiCtrl
				subq.l	#2,a0
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)
				move.l	(a7)+,a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	v_getbitmap_info
				
				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#239,d1
				bsr		_VdiCtrl
				move.l	28(a7),a1
				move.w	d0,(a1)
				move.l	32(a7),a1
				move.w	(a0)+,(a1)
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)
				move.l	(a7)+,a1
				move.l	(a0)+,(a1)
				move.l	4(a7),a1
				move.l	(a0)+,(a1)
				move.l	8(a7),a1
				move.l	(a0)+,(a1)
				move.l	28(a7),a1
				move.l	(a0),(a1)
				rts
				
				ENDMOD


				MODULE	vqt_get_table
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				clr.l	intout(a0)
				move.w	#254,d1
				bsr		_VdiCtrl
				subq.l	#2,a0
				move.l	(a7)+,a1
				move.l	(a0),a0
				move.l	a0,(a1)
				rts
				
				ENDMOD


				MODULE	vqt_cachesize
				
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#255,d1
				bsr		_VdiCtrl
				subq.l	#2,a0
				move.l	(a7)+,a1
				move.l	(a0),d0
				move.l	d0,(a1)
				rts
				
				ENDMOD


