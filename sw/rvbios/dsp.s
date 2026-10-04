;-------------------------------------------------------------------------------
; Raven xbios extensions
; (c) 2024-2026 Anders Granlund
;
; Parts of this code is derived from CT60 xbios (c) Didier Mequignon
;-------------------------------------------------------------------------------
;
; This file is free software  you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation  either version 2, or (at your option)
; any later version.
;
; This file is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY  without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program  if not, write to the Free Software
; Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
;
;-------------------------------------------------------------------------------

;		0x0060	void Dsp_DoBlock(i8* in, i32 size_in, i8* out, i32 size_out)
;		0x0061	void Dsp_BlkHandShake(i8* in, i32 size_in, i8* out, i32 size_out)
;		0x0062	void Dsp_BlkUnpacked(i32* in, i32 size_in, i32* out, i32_size_out)
;		0x0063	void Dsp_InStream(i8* in, i32 block_size, i32 num_blocks, i32* blocks_done)
;		0x0064	void Dsp_OutStream(i8* out, i32_block_size, i32 num_blocks, i32* blocks_done)
;		0x0065 	void Dsp_IOStream(i8* in, i8* out, i32 block_insize, i32 block_outsize, i32 num_blocks, i32* blocks_done)
;		0x0066	void Dsp_RemoveInterrupts(i16 mask)
;		0x0067	i16  Dsp_GetWordSize(void)
;		0x0068	i16  Dsp_Lock(void)
;		0x0069	void Dsp_Unlock(void)
;		0x006A	void Dsp_Available(i32* xavail, i32* yavail)
;		0x006B	void Dsp_Reserve(i32 xreserve, i32 yreserve)
;		0x006C  i16  Dsp_LoadProg(i8* file, i16 ability, i8* buffer)
;		0x006D  void Dsp_ExecProg(i8* code, i32 codesize, i16 ability)
;		0x006E  void Dsp_ExecBoot(i8* code, i32 codesize, i16 ability)
;		0x006F  i32  Dsp_LodToBinary(i8* file, i8* code)
;		0x0070  void Dsp_TriggerHC(i16 vec)
;		0x0071  i16	 Dsp_RequestUniqueAbility(void)
;		0x0072  i16  Dsp_GetProgAbility(void)
;		0x0073	void Dsp_FlushSubroutines(void)
;		0x0074	i16  Dsp_LoadSubRoutine(i8* code, i32 size, i16 ability)
;		0x0075	i16	 Dsp_InqSubrAbility(i16 ability)
;		0x0076	i16  Dsp_RunSubroutine(i16 handle)
;		0x0077	i16	 Dsp_Hf0(i16 flag)
;		0x0078  i16  Dsp_Hf1(i16 flag)
;		0x0079  i16  Dsp_Hf2(void)
;		0x007A  i16  Dsp_Hf3(void)
;		0x007B  void Dsp_BlkWords(void* in, i32 size_in, void* out, i32 size_out)
;		0x007C	void Dsp_BlkBytes(void* in, i32 size_in, void* out, i32 size_out)
;		0x007D	i8	 Dsp_HStat(void)
;		0x007E	void Dsp_SetVectors(void(*recver)(), i32(*transmitter)())
;		0x007F	void Dsp_MultBlocks(i32 numsend, i32 numrecv, DSPBLOCK* sendblocks, DSPBLOCK* recvblocks)

	.XREF	xbios_old
	.XREF	trap14_table
	.XREF	xbc_dsp_lodtobin
	
	.EXPORT InstallTrap14Dsp
	.EXPORT InstallAvec5Dsp

	.TEXT
	.align


UART1			EQU 0x20000000	; dsp control
UART2			EQU 0x20000020	; dsp interrupts
UART_IER		EQU 0x04		; interrupt enable register
UART_ISR		EQU 0x08		; interrupt status register
UART_LCR		EQU 0x0C		; line control register
UART_MCR		EQU 0x10		; modem control register
UART_MSR		EQU 0x18		; modem status register

DSP_CTRL1		EQU UART1+UART_MCR
DSP_CTRL2		EQU	UART2+UART_MCR

DSP_ICR 		EQU 0x20000200
DSP_CVR 		EQU DSP_ICR+0x04
DSP_ISR 		EQU DSP_ICR+0x08
DSP_IVR 		EQU DSP_ICR+0x0c
DSP_TXX			EQU DSP_ICR+0x10
DSP_TXH			EQU DSP_ICR+0x14
DSP_TXM			EQU DSP_ICR+0x18
DSP_TXL			EQU DSP_ICR+0x1c
DSP_RXX			EQU DSP_TXX
DSP_RXH			EQU DSP_TXH
DSP_RXM			EQU DSP_TXM
DSP_RXL			EQU DSP_TXL

DSP_XSIZE		EQU	0x8000
DSP_YSIZE		EQU 0xff00

;DSP_SUBRT_VEC	EQU	15
;DSP_SUBRT_NUM	EQU 8


;----------------------------------------------------------
;
; DSP interrupts (UART2:MSR, autovector5)
;
;----------------------------------------------------------

	DC.B "XBRA"
	DC.B "RAVN"
ivr_dummy_old:
	DC.L 0x03FC
ivr_dummy_new:
	rte

	DC.B "XBRA"
	DC.B "RAVN"
avec5_old:
	DC.L 0x0074
avec5_new:
	move.l	avec5_old,-(sp)			; default jump target
	move.l	d0,-(sp)				; save d0
	move.b	UART2+UART_ISR,d0		; dsp interrupt?
	and.b	#0x3f,d0				; ISR bit0-5 = 0 = MSR interrupt
	bne.b	.1
	move.b	UART2+UART_MSR,d0		; reading MSR clears interrupt
	or.b	#0x77,d0				; bit7 = #CD Status, bit3 = #CD delta
	not.b	d0						; all zeroes = #CD asserted, #CD changed
	bne.b	.1
	moveq.l	#0,d0
	move.b	DSP_IVR,d0				; dsp interrupt vector
	move.l	(d0.w*4),4(sp)			; fetch address from vectors at #0
.1:	move.l	(sp)+,d0				; restore d0
	rts								; jump interrupt handler

InstallAvec5Dsp:
	move.w	sr,-(sp)
	move.w	#0x2700,sr
	move.l	0x3fc,ivr_dummy_old		; replace ivr handler (dummy)
	move.l	#ivr_dummy_new,0x3fc
	move.l	0x74,avec5_old			; replace autovector5 handler
	move.l	#avec5_new,0x74
	moveq.l	#0,d0
	move.b	UART2+UART_IER,d0		; enable MSR interrupts
	or.b	#0x08,d0
	move.b	d0,UART2+UART_IER
	move.w	(sp)+,sr
	rts


;----------------------------------------------------------
;
; Install
;
;----------------------------------------------------------

InstallTrap14Dsp:
	move.w	sr,-(sp)
	move.w	#0x2700,sr
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

;----------------------------------------------------------		## todo
;		0x0060	void Dsp_DoBlock(i8* in, i32 size_in, i8* out, i32 size_out)
xb_dsp_doblock:
	move.l	(a0)+,a1		; a1 = input buffer
	move.l	(a0)+,d1		; d1 = input size
	beq.b	.3
	subq.l	#1,d1
	move.l	#DSP_TXH,a2
.1:	btst	#1,DSP_ISR		; wait dsp ready
	beq.b	.1
.2:	move.l	(a1),d0
	rol.l	#8,d0
	move.b	d0,0(a2)		; send high
	rol.l	#8,d0
	move.b	d0,4(a2)		; send mid
	rol.l	#8,d0
	move.b	d0,8(a2)		; send low
	addq.l	#3,a1
	dbra	d1,.2
.3:	move.l	(a0)+,a1		; a1 = output buffer
	move.l	(a0),d1			; d1 = output size
	beq.b	.6
	subq.l	#1,d1
	move.l	#DSP_RXH,a2
.4:	btst	#0,DSP_ISR		; wait dsp ready
	beq.b	.4
.5:	move.b	0(a2),(a1)+		; recv high
	move.b	4(a2),(a1)+		; recv mid
	move.b	8(a2),(a1)+		; recv low
	dbra	d1,.5
.6:	rte

;----------------------------------------------------------		## todo
;		0x0061	void Dsp_BlkHandShake(i8* in, i32 size_in, i8* out, i32 size_out)
xb_dsp_blkhandshake:
	move.l	(a0)+,a1		; a1 = input buffer
	move.l	(a0)+,d1		; d1 = input size
	beq.b	.3
	subq.l	#1,d1
	move.l	#DSP_TXH,a2
.1:	btst	#1,DSP_ISR		; wait dsp ready
	beq.b	.1
	move.l	(a1),d0
	rol.l	#8,d0
	move.b	d0,0(a2)		; send high
	rol.l	#8,d0
	move.b	d0,4(a2)		; send mid
	rol.l	#8,d0
	move.b	d0,8(a2)		; send low
	addq.l	#3,a1
	dbra	d1,.1
.3:	move.l	(a0)+,a1		; a1 = output buffer
	move.l	(a0),d1			; d1 = output size
	beq.b	.6
	subq.l	#1,d1
	move.l	#DSP_RXH,a2
.4:	btst	#0,DSP_ISR		; wait dsp ready
	beq.b	.4
	move.b	0(a2),(a1)+		; recv high
	move.b	4(a2),(a1)+		; recv mid
	move.b	8(a2),(a1)+		; recv low
	dbra	d1,.4
.6:	rte

;----------------------------------------------------------		## todo
;		0x0062	void Dsp_BlkUnpacked(i32* in, i32 size_in, i32* out, i32_size_out)
xb_dsp_blkunpacked:
	move.l	(a0)+,a1		; a1 = input buffer
	move.l	(a0)+,d1		; d1 = input size
	beq.b	.3
	subq.l	#1,d1
	move.l	#DSP_TXH,a2
.1:	btst	#1,DSP_ISR		; wait dsp ready
	beq.b	.1
.2:	move.l	(a1)+,d0
	swap	d0
	move.b	d0,0(a2)		; send high
	rol.l	#8,d0
	move.b	d0,4(a2)		; send mid
	rol.l	#8,d0
	move.b	d0,8(a2)		; send low
	dbra	d1,.2
.3:	move.l	(a0)+,a1		; a1 = output buffer
	move.l	(a0),d1			; d1 = output size
	beq.b	.6
	subq.l	#1,d1
	move.l	#DSP_RXH,a2
.4:	btst	#0,DSP_ISR		; wait dsp ready
	beq.b	.4
.5:	moveq.l	#0,d0			; clear top
	move.b	0(a2),d0		; recv high
	lsl.l	#8,d0
	move.b	4(a2),d0		; recv mid
	lsl.l	#8,d0
	move.b	8(a2),d0		; recv low
	move.l	d0,(a1)+
	dbra	d1,.5
.6:	rte

;----------------------------------------------------------		## todo
;		0x0063	void Dsp_InStream(i8* in, i32 block_size, i32 num_blocks, i32* blocks_done)
xb_dsp_instream:
	move.l	(a0)+,dsp_inbuf
	move.l	(a0)+,d0
	move.l	d0,dsp_ibsize
	move.l	(a0)+,d1
	move.l	d1,dsp_ibcount
	move.l	(a0),a0
	move.l	a0,dsp_ibdone
	move.l	#0,(a0)			; *ibdone = 0
	tst.l	d0				; (ibsize == 0) ?
	beq.b	.1
	tst.l	d1				; (ibcount == 0) ?
	beq.b	.1
	move.l	#dsp_streamvec,0x3fc
	move.b	#0xff,DSP_IVR
	ori.b	#2,DSP_ICR
.1:	rte

;----------------------------------------------------------		## todo
;		0x0064	void Dsp_OutStream(i8* out, i32_block_size, i32 num_blocks, i32* blocks_done)
xb_dsp_outstream:
	move.l	(a0)+,dsp_outbuf
	move.l	(a0)+,d0
	move.l	d0,dsp_obsize
	move.l	(a0)+,d1
	move.l	d1,dsp_obcount
	move.l	(a0),a0
	move.l	a0,dsp_obdone
	move.l	#0,(a0)			; *obdone = 0
	tst.l	d0				; (obsize == 0) ?
	beq.b	.1
	tst.l	d1				; (obcount == 0) ?
	beq.b	.1
	move.l	#dsp_streamvec,0x3fc
	move.b	#0xff,DSP_IVR
	ori.b	#1,DSP_ICR
.1:	rte

dsp_streamvec:
	movem.l	d0-d1/a0-a1,-(sp)
	btst	#0,DSP_ISR
	beq.b	.2
	move.l	#DSP_RXH,a1			; recv one block
	move.l	dsp_outbuf,a0
	move.l	dsp_obsize,d1
	subq.l	#1,d1
.1:	move.b	0(a1),(a0)+			; recv high
	move.b	4(a1),(a0)+			; recv mid
	move.b	8(a1),(a0)+			; recv low
	dbra	d1,.1
	move.l	a0,dsp_outbuf		; update bufptr
	move.l	dsp_obdone,a0		; update done counter
	add.l	#1,(a0)
	move.l	(a0),d0
	cmp.l	dsp_obcount,d0
	bne.b	.4					; not done = exit
	andi.b	#0xfe,DSP_ICR		; done = stop interrupt and exit
	bra.b	.4
.2:	move.l	#DSP_TXH,a1			; send one block
	move.l	dsp_inbuf,a0
	move.l	dsp_ibsize,d1
	subq.l	#1,d1
.3:	move.l	(a0),d0
	rol.l	#8,d0
	move.b	d0,0(a1)			; send high
	rol.l	#8,d0
	move.b	d0,4(a1)			; send mid
	rol.l	#8,d0
	move.b	d0,8(a1)			; send low
	addq.l	#3,a0
	dbra	d1,.3
	move.l	a0,dsp_inbuf		; update bufptr
	move.l	dsp_ibdone,a0		; update done counter
	addq.l	#1,(a0)
	move.l	(a0),d0
	cmp.l	dsp_ibcount,d0
	bne.b	.4					; not done, exit
	andi.b	#0xfd,DSP_ICR		; done = stop interrupt
.4:	movem.l	(sp)+,d0-d1/a0-a1
	rte

;----------------------------------------------------------		## todo
;		0x0065 	void Dsp_IOStream(i8* in, i8* out, i32 block_insize, i32 block_outsize, i32 num_blocks, i32* blocks_done)
xb_dsp_iostream:
	move.l	(a0)+,a1
	move.l	a1,dsp_inbuf
	move.l	(a0)+,dsp_outbuf
	move.l	(a0)+,d1
	move.l	d0,dsp_ibsize
	move.l	(a0)+,dsp_obsize
	move.l	(a0)+,dsp_ibcount
	move.l	(a0)+,a0
	move.l	a0,dsp_ibdone
	move.l	#0,(a0)					; clear done counter
	move.l	#DSP_TXH,a2
	subq.l	#1,d1					; send block
.1:	move.l	(a1),d0
	rol.l	#8,d0
	move.b	d0,0(a2)				; send high
	rol.l	#8,d0
	move.b	d0,4(a2)				; send mid
	rol.l	#8,d0
	move.b	d0,8(a2)				; send low
	addq.l	#3,a1
	dbra	d1,.1
	move.l	a1,dsp_inbuf			; update bufptr
	move.l	#dsp_iostreamvec,0x3fc	; enable ints
	move.b	#0xff,DSP_IVR
	ori.b	#1,DSP_ICR
	rte

dsp_iostreamvec:
	movem.l	d0-d1/a0-a1,-(sp)
	move.l	dsp_outbuf,a0			; recv block
	move.l	dsp_obsize,d1
	move.l	#DSP_RXH,a1
	subq.l	#1,d1
.1:	move.b	0(a1),(a0)+				; recv high
	move.b	4(a1),(a0)+				; recv mid
	move.b	8(a1),(a0)+				; recv low
	dbra	d1,.1
	move.l	a0,dsp_outbuf			; update bufptr
	move.l	dsp_ibdone,a0			; update done counter
	addq.l	#1,(a0)
	move.l	(a0),d0
	cmp.l	dsp_ibcount,d0
	bne.b	.2
	andi.b	#0xfe,DSP_ICR			; disable int and exit
	bra.b	.4
.2:	move.l	dsp_inbuf,a0			; send block
	move.l	dsp_ibsize,d1
	move.l	#DSP_TXH,a1
	subq.l	#1,d1
.3:	move.l	(a0),d0
	rol.l	#8,d0
	move.b	d0,0(a1)				; send high
	rol.l	#8,d0
	move.b	d0,4(a1)				; send mid
	rol.l	#8,d0
	move.b	d0,8(a1)				; send low
	addq.l	#3,a0
	dbra	d1,.3
	move.l	a0,dsp_inbuf			; update bufptr
.4:	movem.l	(sp)+,d0-d1/a0-a1
	rte

;----------------------------------------------------------		## todo
;		0x0066	void Dsp_RemoveInterrupts(i16 mask)
xb_dsp_removeinterrupts:
	move.w	(a0),d0
	and.b	#3,d0	; 1=recv, 2=send, 3=all
	not.b	d0
	and.b	d0,DSP_ICR
	rte

;----------------------------------------------------------
;		0x0067	i16  Dsp_GetWordSize(void)
xb_dsp_getwordsize:
	move.w	#3,d0
	rte

;----------------------------------------------------------
;		0x0068	i16  Dsp_Lock(void)
xb_dsp_lock:
	move.w	dsp_locked,d0
	bne.b	.1
	move.w	#0xffff,dsp_locked
.1:	rte

;----------------------------------------------------------
;		0x0069	void  Dsp_Unlock(void)
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
	move.l	6(a0),a1	; buffer
	move.l	(a0),a0		; filename
	bsr.l	xbc_dsp_lodtobin
	move.l	(sp)+,a0
	; verify and run binary
	cmp.l	#0,d0		; size
	ble.b	.1
	move.w	4(a0),d1	; ability
	move.l	6(a0),a0	; code
	bsr.w	dsp_execprog
	moveq.l	#0,d0
	rte
.1:	move.l	#-1,d0
	rte

;----------------------------------------------------------		## todo
;		0x006D  void Dsp_ExecProg(i8* code, i32 codesize, i16 ability)
dsp_execprog:
	move.l	d0,-(sp)
	move.l	a0,-(sp)
	move.w	d1,dsp_program	; set current ability

	; upload and start bootloader
	moveq.l	#0,d0
	move.w	dsp_bootloader+7,d0
	move.l	#dsp_bootloader+9,a0
	bsr.w	dsp_execboot

.1:; wait for bootloader ready to recieve
	btst.b	#3,DSP_ISR	; HF2
	beq.b	.1

	; load program
	move.l	(sp)+,a0
	move.l	(sp)+,d0
	beq.b	.5

	bra.b	.3
.2:	btst.b	#1,DSP_ISR
	beq.b	.2
	move.b	(a0)+,DSP_TXH
	move.b	(a0)+,DSP_TXM
	move.b	(a0)+,DSP_TXL
.3:	dbra.w	d0,.2

	; run program
.4:	btst.b	#1,DSP_ISR
	beq.b	.4
	move.b	#0,DSP_TXH
	move.b	#0,DSP_TXM
	move.b	#3,DSP_TXL
.5:	rts

xb_dsp_execprog:
	move.w	8(a0),d1
	move.l	4(a0),d0
	move.l	(a0),a0
	bsr.w	dsp_execprog
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
	; config
	move.b	#0xff,DSP_IVR		; host interrupt vector 0xff (0x3fc)
	; send
	move.l	d1,d0				; size
	bsr		dsp_send_d0
	move.l	#0,d0				; org
	bsr		dsp_send_d0
	move.l	a1,a0
	move.l	d1,d0
	bsr.w	dsp_boot_send		; data
	bsr.w	dsp_reset_delay
	rts

dsp_reset_delay:
	move.w	#50000,d0
.1:	nop
	nop
	dbra.w	d0,.1
	rts

macro dsp_boot_delay
	nop ; slow until pll has been configured
	nop
endm

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

dsp_send_d0:
.1:	btst.b	#1,DSP_ISR
	dsp_boot_delay
	beq.b	.1
	swap	d0
	dsp_boot_delay
	move.b	d0,DSP_TXH
	rol.l	#8,d0
	dsp_boot_delay
	move.b	d0,DSP_TXM
	rol.l	#8,d0
	dsp_boot_delay
	move.b	d0,DSP_TXL
	rts

xb_dsp_execboot:
	move.l	4(a0),d0
	move.l	(a0),a0
	bsr.w	dsp_execboot
	rte

;----------------------------------------------------------		## todo
;		0x006F  i32  Dsp_LodToBinary(i8* file, i8* code)
xb_dsp_lodtobinary:
	move.l	4(a0),a1
	move.l	(a0),a0
	bsr.l	xbc_dsp_lodtobin
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
	move.l	(a0)+,a1		; a1 = input buffer
	move.l	(a0)+,d1		; d1 = input size
	beq.b	.3
	subq.l	#1,d1
	move.l	#DSP_TXH,a2		; a2 = DSP_TXH
.1:	btst	#1,DSP_ISR		; wait for dsp recv ready
	beq.b	.1
.2:	move.w	(a1)+,d0
	ext.l	d0				; sign extend word->long
	swap	d0				; send full 24bit dsp word
	move.b	d0,0(a2)
	rol.l	#8,d0
	move.b	d0,4(a2)
	rol.l	#8,d0
	move.b	d0,8(a2)
	dbra	d1,.2
.3:	move.l	(a0)+,a1		; a1 = output buffer
	move.l	(a0),d1			; d1 = output size
	beq.b	.6
	subq.l	#1,d1
	move.l	#DSP_RXH,a2
.4:	btst	#0,DSP_ISR		; wait for dsp send ready
	beq.b	.4
.5:	move.b	4(a2),d0		; recv low 16bits
	lsl.w	#8,d0
	move.b	8(a2),d0
	move.w	d0,(a1)+
	dbra	d1,.5
.6:	rte

;----------------------------------------------------------		## todo
;		0x007C	void Dsp_BlkBytes(void* in, i32 size_in, void* out, i32 size_out)
xb_dsp_blkbytes:
	move.l	(a0)+,a1		; a1 = input buffer
	move.l	(a0)+,d1		; d1 = input size
	beq.b	.3
	move.l	#DSP_TXH,a2		; a2 = DSP_TXH
	moveq.l	#0,d0			; d0 = zero
	subq.l	#1,d1
	move.b	d0,0(a2)		; clear upper 16bits of dsp word
	move.b	d0,4(a2)
.1:	btst	#1,DSP_ISR		; wait for dsp recv ready
	beq.b	.1
.2:	move.b	(a1)+,8(a2)		; send the low 8bits
	dbra	d1,.2
.3:	move.l	(a0)+,a1		; a1 = output buffer
	move.l	(a0),d1			; d1 = output size
	beq.b	.6
	subq.l	#1,d1
	move.l	#DSP_RXL,a2		; a2 = DSP_RXL
.4:	btst	#0,DSP_ISR		; wait for dsp send ready
	beq.b	.4
.5:	move.b	(a2),(a1)+		; recv low 8bits 
	dbra	d1,.5
.6:	rte

;----------------------------------------------------------
;		0x007D	i8	 Dsp_HStat(void)
xb_dsp_hstat:
	move.b	DSP_ISR,d0
	move.b	DSP_CVR,d1
	rte

;----------------------------------------------------------		## todo
;		0x007E	void Dsp_SetVectors(void(*recver)(), i32(*transmitter)())
xb_dsp_setvectors:
	move.l	#0,dsp_sendfun			; clear send func
	move.l	#0,dsp_recvfun			; clear recv func
	move.l	(a0)+,d0				; d0 = recv fun
	beq.b	.1
	move.l	d0,dsp_recvfun
	move.l	#dsp_hreqvec,0x3fc		; vec 255 function
	move.b	#0xff,DSP_IVR			; set vec 255 on dsp
	ori.b	#1,DSP_ICR				; rxdf request enable
.1:	move.l	(a0),d0					; d0 = send fun
	beq.b	.2
	move.l	#dsp_hreqvec,0x3fc		; vec 255 function
	move.b	#0xff,DSP_IVR			; set vec 255 on dsp
	ori.b	#2,DSP_ICR				; txde request enable
.2:	rte

dsp_hreqvec:
	movem.l	d0-d2/a0-a2,-(sp)
	btst.b	#0,DSP_ISR				; recv request?
	beq.s	.1
	move.l	dsp_recvfun,d0
	beq.s	.1
	move.l	d0,a0					; a0 = recv func
	moveq.l	#0,d0					; d0 = dsp word
	move.b	DSP_RXH,d0
	rol.l	#8,d0
	move.b	DSP_RXM,d0
	rol.l	#8,d0
	move.b	DSP_RXL,d0
	move.l	d0,-(sp)				; recvfun(data)
	jsr		(a0)
	addq.l	#4,sp
.1:	btst.b	#1,DSP_ISR				; send request?
	beq.s	.2
	move.l	dsp_sendfun,d0
	beq.s	.2
	move.l	d0,a0
	jsr		(a0)					; d0 = sendfun()
	tst.l	d0						; send if d0 != 0
	beq.s	.2
	swap	d0
	move.b	d0,DSP_TXH
	rol.l	#8,d0
	move.b	d0,DSP_TXM
	rol.l	#8,d0
	move.b	d0,DSP_TXL
.2:	movem.l	(sp)+,d0-d2/a0-a2
	rte

;----------------------------------------------------------		## todo
;		0x007F	void Dsp_MultBlocks(i32 numsend, i32 numrecv, DSPBLOCK* sendblocks, DSPBLOCK* recvblocks)
xb_dsp_multblocks:
	movem.l	d2-d3/a2-a3,-(sp)
	move.l	(a0),d2			; d2 = block count
	beq.w	xb_dsp_multblocks_recv
	move.l	8(a0),a2		; a2 = block info
	subq.l	#1,d2
.1:	btst	#1,DSP_ISR		; wait dsp ready
	beq.b	.1
.2:	move.w	(a2)+,d2		; d2 = block type
	move.l	(a2)+,d1		; d1 = block size
	move.l	(a2)+,a1		; a1 = block data
	move.l	#DSP_TXH,a3		; a3 = dsp port
	subq.l	#1,d1
	cmp.w	#0,d2
	beq.b	.3				; longs
	cmp.w	#1,d2
	beq.b	.4				; words
	cmp.w	#2,d2
	beq.b	.5				; chars
	bra.w	xb_dsp_multblocks_done
.3:	move.l	(a1)+,d0		; long
	swap	d0
	move.b	d0,0(a3)		; send high
	rol.l	#8,d0
	move.b	d0,4(a3)		; send mid
	rol.l	#8,d0
	move.b	d0,8(a3)		; send low
	dbra	d1,.3			; next data
	dbra	d2,.2			; next block
	bra.b	xb_dsp_multblocks_recv
.4:	move.w	(a1)+,d0		; short
	ext.l	d0
	swap	d0
	move.b	d0,0(a3)		; send high
	rol.l	#8,d0
	move.b	d0,4(a3)		; send mid
	rol.l	#8,d0
	move.b	d0,8(a3)		; send low
	dbra	d1,.4			; next data
	dbra	d2,.2			; next block
	bra.b	xb_dsp_multblocks_recv
.5:	move.b	#0,0(a3)		; send high
	move.b	#0,4(a3)		; send mid
.6:	move.b	(a1)+,8(a3)		; send low
	dbra	d1,.6			; next data
	dbra	d2,.2			; next block

xb_dsp_multblocks_recv:
	move.l	4(a0),d2		; d2 = block count
	beq.b	xb_dsp_multblocks_done
	move.l	12(a0),a2		; a2 = block info
	subq.l	#1,d2
.1:	btst	#0,DSP_ISR		; wait dsp ready
	beq.b	.1
.2:	move.w	(a2)+,d2		; d2 = block type
	move.l	(a2)+,d1		; d1 = block size
	move.l	(a2)+,a1		; a1 = block data
	move.l	#DSP_RXH,a3		; a3 = dsp port
	subq.l	#1,d1
	cmp.w	#0,d2
	beq.b	.3				; longs
	cmp.w	#1,d2
	beq.b	.4				; words
	cmp.w	#2,d2
	beq.b	.5				; chars
	bra.b	xb_dsp_multblocks_done
.3:	moveq.l	#0,d0
	move.b	0(a3),d0		; recv high
	rol.l	#8,d0
	move.b	4(a3),d0		; recv mid
	rol.l	#8,d0
	move.b	8(a3),d0		; recv low
	move.l	d0,(a1)+
	dbra	d1,.3			; next data
	dbra	d2,.2			; next block
	bra.b	xb_dsp_multblocks_done
.4:	move.b	4(a3),d0		; recv mid
	rol.w	#8,d0
	move.b	8(a3),d0		; recv low
	move.w	d0,(a1)+
	dbra	d1,.4			; next data
	dbra	d2,.2			; next block
	bra.b	xb_dsp_multblocks_done
.5:	move.b	8(a3),(a1)+		; recv low
	dbra	d1,.5			; next data
	dbra	d2,.2			; next block

xb_dsp_multblocks_done:
	movem.l	(sp)+,d2-d3/a2-a3
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
dsp_recvfun:	dc.l	0	; app receive func
dsp_sendfun:	dc.l	0	; app send func
dsp_inbuf:		dc.l	0	; app input buffer ptr
dsp_outbuf:		dc.l	0	; app output buffer ptr

dsp_ibsize:		dc.l	0	; input block size
dsp_ibcount:	dc.l	0	; input block count
dsp_ibdone:		dc.l	0	; input blocks done

dsp_obsize:		dc.l	0	; output block size
dsp_obcount:	dc.l	0	; output block count
dsp_obdone:		dc.l	0	; output blocks done


; ------------------------------------------------------------------------
	.align 4
dsp_bootloader:
	.include "../dsp/boot.a56"
dsp_bootloader_end:
	dc.b	0,0,0,0,0,0,0,0,0
	.even
