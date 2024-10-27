				INCLUDE "gem.i"

				GLOBL	vst_load_fonts
				GLOBL	vst_unload_fonts
				GLOBL	v_clrwk
				GLOBL	v_clsvwk
				GLOBL	v_clswk
				GLOBL	v_opnvwk
				GLOBL	v_opnwk
				GLOBL	v_updwk
				GLOBL	vs_clip
				GLOBL	vs_clip_pxy
				GLOBL	vs_clip_off
				GLOBL	v_opnbm
				GLOBL	v_resize_bm
				GLOBL	v_open_bm
				GLOBL	v_clsbm
				GLOBL	v_create_driver_info
				GLOBL	v_delete_driver_info
				GLOBL	v_read_default_settings
				GLOBL	v_write_default_settings
				GLOBL	v_killoutline
				GLOBL	v_getoutline
				GLOBL	vq_devinfo
				GLOBL	vq_ext_devinfo
				GLOBL	v_savecache
				GLOBL	v_loadcache
				GLOBL	v_flushcache
				GLOBL	v_get_outline
				GLOBL	vdipb

				MODULE	vst_load_fonts

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				clr.l   20(a0) /* contrl[10/11] = 0 */
				move.w	d1,intin(a0)
				moveq	#119,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	vst_unload_fonts

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				moveq	#120,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_clrwk

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				moveq	#3,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_clsvwk

				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
		     	clr.w   v_nintin(a0)
		    	moveq   #101,d1
				bra     _VdiCtrl

				ENDMOD


				MODULE	v_clsbm

				lea     _GemParBlk,a0
				clr.w   v_nptsin(a0)
		     	clr.w   v_nintin(a0)
		     	move.w	#1,v_opcode2(a0)
		    	moveq   #101,d1
				bra     _VdiCtrl2

				ENDMOD


				MODULE	v_clswk

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				moveq	#2,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_opnvwk

			if NEW_MT
				move.l	a2,-(a7)
				move.l	a0,d0
				VDIPB 0
				
				lea		vdi_intin(sp),a2
				move.l	d0,a0
				moveq	#11-1,d0
v_opnvwk1:		move.w	(a0)+,(a2)+
				dbf		d0,v_opnvwk1
				move.l	a1,a2
				VDI_CALL (a1), #100, #0, #11
				move.w	vdi_control+v_handle(sp),(a2)
				lea		vdi_intout(sp),a0
				movea.l 12(a6),a1
				moveq	#45-1,d0
v_opnvwk2:		move.w	(a0)+,(a1)+
				dbf	d0,v_opnvwk2
				lea		vdi_ptsout(a7),a0
				moveq	#12-1,d0
v_opnvwk3:		move.w	(a0)+,(a1)+
				dbf		d0,v_opnvwk3
				VDI_END
				move.l	(a7)+,a2
				rts
			else
				move.l	a2,-(a7)
				lea		_GemParBlk,a2
				move.w	(a1),v_handle(a2)
				move.l	a1,-(a7)
				move.w	#100,v_opcode(a2)
				clr.w	v_opcode2(a2)
				clr.w	v_nptsin(a2)
				move.w	#11,v_nintin(a2)
				lea		intin(a2),a2
				moveq	#11-1,d0
v_opnvwk1:		move.w	(a0)+,(a2)+
				dbf		d0,v_opnvwk1
				move.l	#vdipb,d1
				moveq	#$73,d0
				trap	#2
				movea.l (a7)+,a0
				move.w	_GemParBlk+v_handle,(a0)
				lea		_GemParBlk+intout,a0
				movea.l 8(a7),a1
				moveq	#45-1,d0
v_opnvwk2:		move.w	(a0)+,(a1)+
				dbf	d0,v_opnvwk2
				lea		_GemParBlk+ptsout,a0
				moveq	#12-1,d0
v_opnvwk3:		move.w	(a0)+,(a1)+
				dbf		d0,v_opnvwk3
				move.l	(a7)+,a2
				rts
			endc

				ENDMOD


				MODULE	v_opnbm

				move.l	a2,-(a7)
				lea		_GemParBlk,a2
				move.l	a1,v_param(a2)
				move.l	8(a7),a1
				move.w	(a1),v_handle(a2)
				move.w	#100,v_opcode(a2)
				move.w	#1,v_opcode2(a2)
				clr.w	v_nptsin(a2)
				move.w	#20,v_nintin(a2)
				lea		intin(a2),a2
				moveq	#20-1,d0
v_opnbm1:
				move.w	(a0)+,(a2)+
				dbf		d0,v_opnbm1
				move.l	#vdipb,d1
				moveq	#$73,d0
				trap	#2
				lea		_GemParBlk,a2
				movea.l 8(a7),a1
				move.w	v_handle(a2),(a1)
				lea		intout(a2),a2
				movea.l 12(a7),a1
				moveq	#45-1,d0
v_opnbm2:
				move.w	(a2)+,(a1)+
				dbf		d0,v_opnbm2
				lea		_GemParBlk+ptsout,a2
				moveq	#12-1,d0
v_opnbm3:
				move.w	(a2)+,(a1)+
				dbf		d0,v_opnbm3
				move.l	(a7)+,a2
				rts

				ENDMOD


				MODULE	v_resize_bm

				move.l	a0,a1
				lea		_GemParBlk,a0
				move.w	#2,v_opcode2(a0)
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				move.l	4(a7),intin+4(a0)
				move.l	a1,intin+8(a0)
				moveq	#100,d1
				bra		_VdiCtrl2

				ENDMOD


				MODULE	v_open_bm

				move.l	a0,a1
				lea		_GemParBlk,a0
				move.l	a1,v_param(a0)
				move.w	#3,v_opcode2(a0)
				move.l	a1,v_param(a0)
				clr.w	v_nptsin(a0)
				move.w	#4,v_nintin(a0)
				lea		intin(a0),a1
				move.w	d1,(a1)+
				move.w	d2,(a1)+
				move.l	4(a7),(a1)
				moveq	#100,d1
				bsr		_VdiCtrl2
				move.w	_GemParBlk+v_handle,d0
				rts

				ENDMOD


				MODULE	v_opnwk

				clr.w	_GemParBlk+v_handle
				move.l	a1,-(a7)
				lea		_GemParBlk,a1
				move.w	#1,v_opcode(a1)
				clr.w	v_nptsin(a1)
				move.w	#16,v_nintin(a1)
				lea		intin(a1),a1
				moveq	#16-1,d0
v_opnwk1:		move.w	(a0)+,(a1)+
				dbf		d0,v_opnwk1
				move.l	#vdipb,d1
				moveq	#$73,d0
				move.l	a2,-(a7)
				trap	#2
				move.l	(a7)+,a2
				movea.l (a7)+,a0
				move.w	_GemParBlk+v_handle,(a0)
				lea		_GemParBlk+intout,a0
				movea.l 4(a7),a1
				moveq	#45-1,d0
v_opnwk2:		move.w	(a0)+,(a1)+
				dbf		d0,v_opnwk2
				lea		_GemParBlk+ptsout,a0
				moveq	#12-1,d0
v_opnwk3:		move.w	(a0)+,(a1)+
				dbf		d0,v_opnwk3
				rts

				ENDMOD

				MODULE	vdipb

				dc.l	_GemParBlk+control
				dc.l	_GemParBlk+intin
				dc.l	_GemParBlk+ptsin
				dc.l	_GemParBlk+intout
				dc.l	_GemParBlk+ptsout

				ENDMOD


				MODULE	v_updwk

			if NEW_MT
				VDIPB 0
				VDI_JUMP d0, #4, #0, #0
			else
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				moveq	#4,d1
				bra		_VdiCtrl
			endc

				ENDMOD


				MODULE	vs_clip

			if NEW_MT
				move.l	a0,d2
				VDIPB 0
				lea     vdi_intin(sp),a1
				move.w	d1,(a1)
				lea		vdi_ptsin(sp),a1
				move.l	d2,a0
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				VDI_JUMP d0, #129, #2, #1
			else
				lea		_GemParBlk,a1
				move.w	#2,v_nptsin(a1)
				move.w	#1,v_nintin(a1)
				move.w	d1,intin(a1)
				lea		ptsin(a1),a1
				move.l	(a0)+,(a1)+
				move.l	(a0),(a1)
				lea		_GemParBlk,a0
				move.w	#129,d1
				bra		_VdiCtrl
			endc
				
				ENDMOD
				
				
				MODULE	vs_clip_pxy

				move.l	a0,d1
				VDIPB 0
				lea     vdi_intin(sp),a1
				move.w	#1,(a1)
				lea		vdi_ptsin(sp),a1
				move.l	d1,a0
				move.l	(a0)+,(a1)+
				move.l	(a0)+,(a1)+
				VDI_JUMP d0, #129, #2, #1

				ENDMOD


				MODULE	vs_clip_off

				VDIPB 0
				lea     vdi_intin(sp),a1
				clr.w	(a1)
				lea		vdi_ptsin(sp),a1
				clr.l	(a1)+
				clr.l	(a1)+
				VDI_JUMP d0, #129, #2, #1

				ENDMOD


				MODULE	v_create_driver_info

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#180,d1
				bsr		_VdiCtrl
				move.l	-2(a0),d0
				cmp.w	#2,_GemParBlk+v_nintout
				bge		v_create1
				clr.l	d0
v_create1:		move.l	d0,a0
				rts

				ENDMOD


				MODULE	v_delete_driver_info

				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	a1,intin(a0)
				move.w	#181,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_read_default_settings

				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	a1,intin(a0)
				move.w	#182,d1
				bsr		_VdiCtrl
				cmp.w	#1,_GemParBlk+v_nintout
				bge		v_read1
				clr.w	d0
v_read1:		rts

				ENDMOD


				MODULE	v_write_default_settings

				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	a1,intin(a0)
				move.w	#182,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				cmp.w	#1,_GemParBlk+v_nintout
				bge		v_write1
				clr.w	d0
v_write1:		rts

				ENDMOD


				MODULE	v_killoutline

				move.l	a0,d0
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#2,v_nintin(a0)
				move.l	d0,intin(a0)
				move.w	#242,d1
				bra		_VdiCtrl

				ENDMOD



				MODULE	v_getoutline

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#6,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	d2,intin+2(a0)
				move.l	a1,intin+4(a0)
				move.l	(a7)+,intin+8(a0)
				move.w	#243,d1
				bsr		_VdiCtrl
				cmp.w	#1,_GemParBlk+v_nintout
				bge		v_getoutline1
				clr.w	d0
v_getoutline1:	move.l	4(a7),a1
				move.w	d0,(a1)
				rts

				ENDMOD



				MODULE	v_get_outline

				move.l	a1,-(a7)
				move.l	a0,a1
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#8,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	6(a7),intin+2(a0)
				move.l	a1,intin+4(a0)
				move.l	(a7)+,intin+8(a0)
				move.w	d2,intin+12(a0)
				move.w	4(a7),intin+14(a0)
				move.w	#243,d1
				move.w	#1,v_opcode2(a0)
				bsr		_VdiCtrl2
				cmp.w	#1,_GemParBlk+v_nintout
				bge		v_get_outline1
				clr.w	d0
v_get_outline1:	rts

				ENDMOD


				MODULE	vq_devinfo

				move.l	a1,-(a7)
				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintout(a0)
				clr.w	v_nptsout(a0)
				move.w	#1,v_nintin(a0)
				move.w	d1,intin(a0)
				move.w	#248,d1
				bsr		_VdiCtrl
				lea		_GemParBlk,a0
				move.l	(a7)+,a1
				move.w	v_nintout(a0),d2
				beq		vq_devinfo1
				tst.w	d0
				bne		vq_devinfo2
vq_devinfo1:	clr.w	d0
				move.w	d0,(a1)
				move.l	(a7)+,a1
				move.b	d0,(a1)
				move.l	4(a7),a1
				move.b	d0,(a1)
				rts
vq_devinfo2:	move.w	ptsout(a0),(a1)
				move.l	(a7)+,a1
				; set the filename. The value in vdi_intout may be "DRIVER.SYS"
				; or "DRIVER  SYS". vdi_intout is not a nul-terminated string.
				; In both cases, this binding returns a valid filename: "DRIVER.SYS"
				; with a null-character to ended the string.
				lea		intout(a0),a0
				bra		vq_devinfo6
vq_devinfo3:	move.w	(a0)+,d0
				move.b	d0,(a1)
				beq		vq_devinfo7
				cmp.b	#32,d0
				bne		vq_devinfo5
				tst.w	d2
				beq		vq_devinfo6
				cmp.b	#32,1(a0)
				beq		vq_devinfo6
				cmp.b	#46,1(a0)
				beq		vq_devinfo6
				move.b	#46,(a1)
vq_devinfo5:	addq.l	#1,a1
vq_devinfo6:	dbf		d2,vq_devinfo3
vq_devinfo7:	clr.b	(a1)
				; device name in ptsout is a C-String,
				; (a nul-terminated string with 8bits per characters)
				; each short value (vdi_ptsout[x]) contains 2 characters.
				; When ptsout contains a device name, NVDI/SpeedoGDOS
				; seems to always write the value "13"
				; in vdi_control[1] (hey! this should be a read only
				; value from the VDI point of view!!!),
				; and SpeedoGDOS 5 may set vdi_control[2] == 1
				; (instead of the size of vdi_ptsout, including
				; the device_name). It's seems that this value "13"
				; (written in vdi_control[1]) has missed
				; its target (vdi_control[2]). So, here is a workaround:
				lea		_GemParBlk,a0
				move.l	4(a7),a1
				move.w	v_nptsout(a0),d2
				beq		vq_devinfo11
				sub.w	#1,d2
				bne		vq_devinfo8
				move.w	v_nptsin(a0),d0
				ble		vq_devinfo8
				move	d0,d2
vq_devinfo8:	add.w	d2,d2
				lea		ptsout+2(a0),a0
				bra		vq_devinfo10
vq_devinfo9:	move.b	(a0)+,(a1)+
vq_devinfo10:	dbf		d2,vq_devinfo9
vq_devinfo11:	clr.b	(a1)
				rts

				ENDMOD


				MODULE	vq_ext_devinfo

				move.l	a0,-(a7)
				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				move.w	#7,v_nintin(a0)
				move.w	d1,intin(a0)
				move.l	a1,intin+2(a0)
				move.l	8(a7),intin+6(a0)
				move.l	12(a7),intin+10(a0)
				move.w	#248,d1
				move.w	#4242,v_opcode2(a0)
				bsr		_VdiCtrl2
				move.l	(a7)+,a1
				move.w	d0,(a1)
				move.w	(a0),d0
				rts

				ENDMOD


				MODULE	v_savecache

				lea		_GemParBlk,a1
				clr.w	v_nptsin(a1)
				lea		intin(a1),a1
				moveq	#0,d1
				moveq	#-1,d2
v_savecache1:	move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_savecache1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#249,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_loadcache

				lea		_GemParBlk,a1
				clr.w	v_nptsin(a1)
				lea		intin(a1),a1
				move.w	d1,(a1)+
				moveq	#0,d1
				moveq	#0,d2
v_loadcache1:	move.b	(a0)+,d1
				addq.w	#1,d2
				move.w	d1,(a1)+
				bne		v_loadcache1
				lea		_GemParBlk,a0
				move.w	d2,v_nintin(a0)
				move.w	#250,d1
				bra		_VdiCtrl

				ENDMOD


				MODULE	v_flushcache

				lea		_GemParBlk,a0
				clr.w	v_nptsin(a0)
				clr.w	v_nintin(a0)
				move.w	#251,d1
				bra		_VdiCtrl

				ENDMOD


