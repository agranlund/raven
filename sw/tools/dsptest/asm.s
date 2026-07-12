; ------------------------------------------------------------------------
; dsp xbios testbed for faster iteration
; parts of this should normally come with rvbios
; ------------------------------------------------------------------------

DSP_CTRL1		EQU 0x20000010			; uart2:mcr
DSP_CTRL2		EQU	0x20000030			; uart1:mcr

DSP_ICR 		EQU 0x20000400
DSP_CVR 		EQU DSP_ICR+0x04
DSP_ISR 		EQU DSP_ICR+0x08
DSP_IVR 		EQU DSP_ICR+0x0c
DSP_TXH			EQU DSP_ICR+0x14
DSP_TXM			EQU DSP_ICR+0x18
DSP_TXL			EQU DSP_ICR+0x1c
DSP_RXH			EQU DSP_TXH
DSP_RXM			EQU DSP_TXM
DSP_RXL			EQU DSP_TXL

DSP_XSIZE		EQU 0xFF00
DSP_YSIZE		EQU 0x8000


; ------------------------------------------------------------------------
; misc asm functions for convenience
; ------------------------------------------------------------------------
	.text
	.align

	.EXPORT readbuf

readbuf:
	bra.b	.2
.1:	btst.b	#0,DSP_ISR
	beq.b	.1
	move.b	DSP_RXH,(a0)+
	move.b	DSP_RXM,(a0)+
	move.b	DSP_RXL,(a0)+
.2:	dbra.w	d0,.1
	rts


; ------------------------------------------------------------------------
; here as support only
; functionality already inside rvbios
; ------------------------------------------------------------------------
	.text
	.align

	.EXPORT InstallTrap14

trap14_table:	ds.l	256

InstallTrap14:
	move.w	sr,-(sp)
	move.w	#0x2700,sr
	move.l	0x0b8.w,a0
	move.l	-8(a0),d0
	cmp.l	#0xABBA,d0
	bne.b	.1
	move.l	-4(a0),0x08b8.w
.1:	move.l	0x0b8.w,xbios_old
	move.l	#xbios_new,0x0b8.w
	move.l	#trap14_table,a0
	move.w	#255,d1
.2:	move.l	#xbios_org,(a0)+
	dbra.w	d1,.2
	move.l	#xb_dsp_doblock,				trap14_table + ( 96*4)
	move.l	#xb_dsp_blkhandshake,			trap14_table + ( 97*4)
	move.l	#xb_dsp_blkunpacked,			trap14_table + ( 98*4)
	move.l	#xb_dsp_instream,				trap14_table + ( 99*4)
	move.l	#xb_dsp_outstream,				trap14_table + (100*4)
	move.l	#xb_dsp_iostream,				trap14_table + (101*4)
	move.l	#xb_dsp_removeinterrupts,		trap14_table + (102*4)
	move.l	#xb_dsp_getwordsize,			trap14_table + (103*4)
	move.l	#xb_dsp_lock,					trap14_table + (104*4)
	move.l	#xb_dsp_unlock,					trap14_table + (105*4)
	move.l	#xb_dsp_available,				trap14_table + (106*4)
	move.l	#xb_dsp_reserve,				trap14_table + (107*4)
	move.l	#xb_dsp_loadprog,				trap14_table + (108*4)
	move.l	#xb_dsp_execprog,				trap14_table + (109*4)
	move.l	#xb_dsp_execboot,				trap14_table + (110*4)
	move.l	#xb_dsp_lodtobinary,			trap14_table + (111*4)
	move.l	#xb_dsp_triggerhc,				trap14_table + (112*4)
	move.l	#xb_dsp_requestuniqueability,	trap14_table + (113*4)
	move.l	#xb_dsp_getprogability,			trap14_table + (114*4)
	move.l	#xb_dsp_flushsubroutines,		trap14_table + (115*4)
	move.l	#xb_dsp_loadsubroutine,			trap14_table + (116*4)
	move.l	#xb_dsp_inqsubrability,			trap14_table + (117*4)
	move.l	#xb_dsp_runsubroutine,			trap14_table + (118*4)
	move.l	#xb_dsp_hf0,					trap14_table + (119*4)
	move.l	#xb_dsp_hf1,					trap14_table + (120*4)
	move.l	#xb_dsp_hf2,					trap14_table + (121*4)
	move.l	#xb_dsp_hf3,					trap14_table + (122*4)
	move.l	#xb_dsp_blkwords,				trap14_table + (123*4)
	move.l	#xb_dsp_blkbytes,				trap14_table + (124*4)
	move.l	#xb_dsp_hstat,					trap14_table + (125*4)
	move.l	#xb_dsp_setvectors,				trap14_table + (126*4)
	move.l	#xb_dsp_multblocks,				trap14_table + (127*4)
	move.w	(sp)+,sr
	rts

	DC.L 0
	DC.B "XBRA"
	DC.L 0xABBA
xbios_old:
	DC.L 0x00B8
xbios_new:
	move	usp,a0
	btst	#5,(sp)			; already super?
	beq.b	.1
	lea		8(sp),a0		; assume longframe
.1:	moveq	#0,d0
	move.w	(a0)+,d0		; d0 = opcode, a0 = args

	cmp.w	#256,d0
	bcc.b	xbios_org
	jmp		([trap14_table, d0.w*4])

xbios_org:
	movea.l	xbios_old(pc),a0
	jmp		(a0)




; ------------------------------------------------------------------------
;
; dsp xbios implementation
; should normally live in rvbios
;
; ------------------------------------------------------------------------
	.text
	.align

;----------------------------------------------------------		## todo
;		0x0060	void Dsp_DoBlock(i8* in, i32 size_in, i8* out, i32 size_out)
xb_dsp_doblock:
	rte

;----------------------------------------------------------		## todo
;		0x0061	void Dsp_BlkHandShake(i8* in, i32 size_in, i8* out, i32 size_out)
xb_dsp_blkhandshake:
	rte

;----------------------------------------------------------		## todo
;		0x0062	void Dsp_BlkUnpacked(i32* in, i32 size_in, i32* out, i32_size_out)
xb_dsp_blkunpacked:
	rte

;----------------------------------------------------------		## todo
;		0x0063	void Dsp_InStream(i8* in, i32 block_size, i32 num_blocks, i32* blocks_done)
xb_dsp_instream:
	rte

;----------------------------------------------------------		## todo
;		0x0064	void Dsp_OutStream(i8* out, i32_block_size, i32 num_blocks, i32* blocks_done)
xb_dsp_outstream:
	rte

;----------------------------------------------------------		## todo
;		0x0065 	void Dsp_IOStream(i8* in, i8* out, i32 block_insize, i32 block_outsize, i32 num_blocks, i32* blocks_done)
xb_dsp_iostream:
	rte

;----------------------------------------------------------		## todo
;		0x0066	void Dsp_RemoveInterrupts(i16 mask)
xb_dsp_removeinterrupts:
	rte

;----------------------------------------------------------
;		0x0067	i16  Dsp_GetWordSize(void)
xb_dsp_getwordsize:
	move.w	#3,d0
	rte

;----------------------------------------------------------
;		0x0068	i16  Dsp_Lock(void)
xb_dsp_lock:
	or.w	#0x0700,sr
	move.w	dsp_locked,d0
	bne.b	.1
	move.w	#0xffff,dsp_locked
.1:	rte

;----------------------------------------------------------
;		0x0069	i16  Dsp_Unlock(void)
xb_dsp_unlock:
	or.w	#0x0700,sr
	move.w	#0,dsp_locked
	rte

;----------------------------------------------------------
;		0x006A	void Dsp_Available(i32* xavail, i32* yavail)
xb_dsp_available:
	move.l	(a0)+,a1
	move.l	#DSP_XSIZE,(a1)
	move.l	(a0),a1
	move.l	#DSP_YSIZE,(a1)
	rte

;----------------------------------------------------------		## todo
;		0x006B	void Dsp_Reserve(i32 xreserve, i32 yreserve)
xb_dsp_reserve:
	rte

;----------------------------------------------------------		## todo
;		0x006C  i16  Dsp_LoadProg(i8* file, i16 ability, i8* buffer)
xb_dsp_loadprog:
	; convert lod to binary
	move.l	a0,-(sp)
	move.l	6(a0),a1
	move.l	(a0),a0
	;jsr	dsp_lodtobin	; todo
	; run binary
	cmp.l	#9,d0	; validate size
	ble.b	.1
	move.l	(sp)+,a0
	move.w	4(a0),d1
	move.l	6(a0),a0
	;bsr.w	dsp_execprog
.1:	rte

;----------------------------------------------------------		## todo
;		0x006D  void Dsp_ExecProg(i8* code, i32 codesize, i16 ability)
dsp_execprog:
	move.l	a0,-(sp)
	move.l	d0,-(sp)
	move.w	d1,dsp_program	; set current ability

	; run bootloader
	moveq.l	#0,d0
	move.w	dsp_bootloader+7,d0
	move.l	#dsp_bootloader+9,a0
	jsr		dsp_execboot

	; load program
	move.l	(sp)+,d0
	move.l	(sp)+,a0
	bra.b	.2
.1:	btst.b	#1,DSP_ISR
	beq.b	.1
	move.b	(a0)+,DSP_TXH
	move.b	(a0)+,DSP_TXM
	move.b	(a0)+,DSP_TXL
.2:	dbra.w	d0,.1

	; run program
.3:	btst.b	#1,DSP_ISR
	beq.b	.3
	move.b	#0,DSP_TXH
	move.b	#0,DSP_TXM
	move.b	#3,DSP_TXL
	rts

xb_dsp_execprog:
	move.w	8(a0),d1
	move.l	4(a0),d0
	move.l	(a0),a0
	jsr		dsp_execprog
	rte

;----------------------------------------------------------
;		0x006E  void Dsp_ExecBoot(i8* code, i32 codesize, i16 ability)
dsp_execboot:
	move.l	a0,a1				; a1 = data
	move.l	d0,d1				; d1 = size
	; reset
	and.b	#0xfd,DSP_CTRL1		; dsp off
	bsr.w	dsp_reset_delay
	or.b	#0x02,DSP_CTRL1		; dsp on
	bsr.w	dsp_reset_delay
	; send
	move.l	d1,d0				; size
	bsr		dsp_send_d0
	move.l	#0,d0				; org
	bsr		dsp_send_d0
	move.l	a1,a0
	move.l	d1,d0
	bsr.w	dsp_boot_send		; data
	rts

	; can't go too fast before bootcode has configured pll
macro dsp_boot_delay
	nop
	nop
endm

dsp_reset_delay:
	move.w	#50000,d0
.1:	nop
	nop
	dbra.w	d0,.1
	rts

dsp_send_d0:
.1:	btst.b	#1,DSP_ISR
	beq.b	.1
	swap	d0
	move.b	d0,DSP_TXH
	rol.l	#8,d0
	move.b	d0,DSP_TXM
	rol.l	#8,d0
	move.b	d0,DSP_TXL
	rts

dsp_boot_send:
.1:	btst.b	#1,DSP_ISR
	dsp_boot_delay
	bne.b	.3
	bra.b	.1
.2:	move.b	(a0)+,DSP_TXH
	dsp_boot_delay
	move.b	(a0)+,DSP_TXM
	dsp_boot_delay
	move.b	(a0)+,DSP_TXL
	dsp_boot_delay
.3:	dbra.w	d0,.2
	rts

xb_dsp_execboot:
	move.l	4(a0),d0
	move.l	(a0),a0
	bsr.w	dsp_execboot
	rte

;----------------------------------------------------------		## todo
;		0x006F  i32  Dsp_LodToBinary(i8* file, i8* code)
xb_dsp_lodtobinary:
	rte

;----------------------------------------------------------
;		0x0070  void Dsp_TriggerHC(i16 vec)
xb_dsp_triggerhc:
	or.w	#0x0700,sr
	move.w	(a0),d0
	or.b	#0x80,d0
	move.b	d0,DSP_CVR
	rte

;----------------------------------------------------------
;		0x0071  i16	 Dsp_RequestUniqueAbility(void)
xb_dsp_requestuniqueability:
	or.w	#0x0700,sr
	move.w	dsp_ability,d0
	add.w	#1,d0
	move.w	d0,dsp_ability
	rte

;----------------------------------------------------------
;		0x0072  i16  Dsp_GetProgAbility(void)
xb_dsp_getprogability:
	move.w	dsp_program,d0
	rte

;----------------------------------------------------------		## todo
;		0x0073	void Dsp_FlushSubroutines(void)
xb_dsp_flushsubroutines:
	rte

;----------------------------------------------------------		## todo
;		0x0074	i16  Dsp_LoadSubRoutine(i8* code, i32 size, i16 ability)
xb_dsp_loadsubroutine:
	moveq	#0,d0
	rte

;----------------------------------------------------------		## todo
;		0x0075	i16	 Dsp_InqSubrAbility(i16 ability)
xb_dsp_inqsubrability:
	moveq	#0,d0
	rte

;----------------------------------------------------------		## todo
;		0x0076	i16  Dsp_RunSubroutine(i16 handle)
xb_dsp_runsubroutine:
IFNE 0
	move.w	(a0),d0		; a0 = vector
	cmp.w	#DSP_SUBRT_NUM,d0
	blt.w	.1
	cmp.w	#DSP_SUBRT_VEC+DSP_SUBRT_NUM,d0
	bgt.w	.1
	move.w	d0,d1
	sub.w	#DSP_SUBRT_VEC,d1
	muls.w	#6,d1			; 2 words per entry
	add.w	#3,d1			; skip jsr
	move.l	#dsp_subvecs,a0	; fetch address
	add.w	d1,a0
	move.b	(a0)+,DSP_TXH	; send address
	move.b	(a0)+,DSP_TXM
	move.b	(a0)+,DSP_TXL
	or.b	#0x80,d0		; trigger host command
	move.b	d0,DSP_CVR
	rte
ENDIF	
.1:	move.w	#-1,d0
	rte

;----------------------------------------------------------
;		0x0077	i16	 Dsp_Hf0(i16 flag)
xb_dsp_hf0:
	tst.w	(a0)
	bmi.b	.2
	beq.b	.1
	or.b	#0x08,DSP_ICR
	rte
.1:	and.b	#0xf7,DSP_ICR
	rte
.2: move.b	DSP_ICR,d0
	lsr.b	#3,d0
	and.w	#1,d0
	rte

;----------------------------------------------------------
;		0x0078  i16  Dsp_Hf1(i16 flag)
xb_dsp_hf1:
	tst.w	(a0)
	bmi.b	.2
	beq.b	.1
	or.b	#0x10,DSP_ICR
	rte
.1:	and.b	#0xef,DSP_ICR
	rte
.2:	move.b	DSP_ICR,d0
	lsr.b	#4,d0
	and.w	#1,d0
	rte

;----------------------------------------------------------
;		0x0079  i16  Dsp_Hf2(void)
xb_dsp_hf2:
	move.b	DSP_ISR,d0
	lsr.b	#3,d0
	and.w	#1,d0
	rte

;----------------------------------------------------------
;		0x007A  i16  Dsp_Hf3(void)
xb_dsp_hf3:
	move.b	DSP_ISR,d0
	lsr.b	#4,d0
	and.w	#1,d0
	rte

;----------------------------------------------------------		## todo
;		0x007B  void Dsp_BlkWords(void* in, i32 size_in, void* out, i32 size_out)
xb_dsp_blkwords:
	rte

;----------------------------------------------------------		## todo
;		0x007C	void Dsp_BlkBytes(void* in, i32 size_in, void* out, i32 size_out)
xb_dsp_blkbytes:
	rte

;----------------------------------------------------------
;		0x007D	i8	 Dsp_HStat(void)
xb_dsp_hstat:
	move.b	DSP_ISR,d0
	move.b	DSP_CVR,d1
	rte

;----------------------------------------------------------		## todo
;		0x007E	void Dsp_SetVectors(void(*recver)(), i32(*transmitter)())
xb_dsp_setvectors:
	rte

;----------------------------------------------------------		## todo
;		0x007F	void Dsp_MultBlocks(i32 numsend, i32 numrecv, DSPBLOCK* sendblocks, DSPBLOCK* recvblocks)
xb_dsp_multblocks:
	rte





;----------------------------------------------------------
	.align 4
dsp_vectors:
	dc.b	0x00,0x00,0x00	; P
	dc.b	0x00,0x00,0x00	; org
	dc.b	0x00,0x00,0x00	; size
dsp_tosvecs:
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00	; subroutine loader
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00	; block mover
dsp_subvecs:
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00	; subroutines
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00
	dc.b	0x00,0xf0,0x80, 0x00,0x00,0x00
	.align 4

dsp_locked:		dc.w	0
dsp_ability:	dc.w	0x8000
dsp_program:	dc.w	0
	.align 4


; ------------------------------------------------------------------------
	.text
	.even
	dc.b 	0	; misalign to make header entries aligned
dsp_bootloader:
	.include "../../dsp/boot.a56"
dsp_bootloader_end:
	dc.b	0,0,0,0,0,0,0,0,0
	.even
