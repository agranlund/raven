;-------------------------------------------------------------------------------
; Raven xbios extensions
; (c) 2024 Anders Granlund
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



;-------------------------------------------------------------------------------
;	TOS1
;		0x0016	void SetTime(u32 time)
;		0x0017	u32  GetTime(void)
;
;	TOS3
;		0x002E	i16  NVMAccess(i16 op, i16 start, i16 count, i8* buf)
;
;	CT60/Milan
;		0x00A0 	i32  CacheCtrl(i16 op, i16 param)
;
;	CT60
;		0xC60C	i32  ct60_cache(i16 mode)
;		0xC60D	i32  ct60_flush_cache(i16 mode)
;		0xC60A	i32  ct60_read_temp(i16 type)
;		0xC60B	i32  ct60_rwparam(i16 mode, i32 type, i16 val)
;		0xC60E	i32  ct60_vmalloc(i16 mode, i32 value)
;-------------------------------------------------------------------------------
	.XREF 	xbc_settime
	.XREF 	xbc_gettime
	.XREF 	xbc_nvmaccess

    .XREF   xbc_cache_flush
    .XREF   xbc_cache_enable
    .XREF   xbc_cache_disable

	.EXPORT InstallXbios

	.TEXT

	
;----------------------------------------------------------
;cache_enable:
;	movec.l	cacr,d0
;	btst.b	#31,d0
;	bne.b	.1
;	move.l	#0xA0C08000,d0		; EDC, ESB, EBC, EIC
;	nop
;	cinva	bc
;	nop
;	movec.l	d0,cacr
;.1:	rts
;
;cache_disable:
;	movec.l	cacr,d0
;	btst.b	#31,d0
;	beq.b	.1
;	moveq.l	#0,d0
;	nop
;	cpusha	bc
;	nop
;	movec.l	d0,cacr
;.1:	rts



;----------------------------------------------------------
xb_cache_ctrl:
	or.w	#0x0700,sr		; disable interrupts
	move.w	2(a0),d0		; d0 = opcode
	move.w	4(a0),d1		; d1 = param
.0:	cmp.w	#0,d0			; query implemented
	bne.b	.1
	moveq.l	#0,d0
	rte
.1:	cmp.w	#1,d0			; flush data cache
	bne.b	.2
    bsr     xbc_cache_flush
	moveq.l	#0,d0
	rte
.2:	cmp.w	#2,d0			; flush instruction cache
	bne.b	.3
    bsr     xbc_cache_flush
	moveq.l	#0,d0
	rte
.3:	cmp.w	#3,d0			; flush both caches
	bne.b	.4
    bsr     xbc_cache_flush
	moveq.l	#0,d0
	rte
.4:	cmp.w	#4,d0			; inquire data cache mode
	bne.b	.5
	movec.l	cacr,d0
	rol.l	#1,d0
	and.l	#1,d0
	rte
.5:	cmp.w	#5,d0			; activate/deactivate data cace
	bne.b	.6
	cmp.w	#0,d1
	bne.b	.5a
	bsr		xbc_cache_disable		; disable all caches like CT60
	bra.b	.5b
.5a:bsr		xbc_cache_enable		; enable all caches like CT60
.5b:moveq.l	#0,d0
	rte
.6:	cmp.w	#6,d0			; inquire instruction cache mode
	bne.b	.7
	movec.l	cacr,d0
	rol.w	#1,d0
	and.l	#1,d0
	rte
.7:	cmp.w	#7,d0			; activate/deactivate instruction cache
	bne.b	.8
	cmp.w	#0,d1
	bne.b	.7a
	bsr		xbc_cache_disable		; disable all caches like CT60
	bra.b	.7b
.7a:bsr		xbc_cache_enable		; enable all caches like CT60
.7b:moveq.l	#0,d0
	rte
.8:	move.l	#-5,d0			; EBADRQ
	rte


;----------------------------------------------------------
xb_ct60_cache:
	or.w	#0x0700,sr		; disable interrupts
	move.w	2(a0),d0
	bmi.b	.2
	bne.b	.1
.0:	bsr		xbc_cache_disable
	bra.b	.2
.1:	bsr		xbc_cache_enable
.2:	movec.l	cacr,d0
	rte


;----------------------------------------------------------
xb_ct60_flush_cache:
	or.w	#0x0700,sr		; disable interrupts
    bsr     xbc_cache_flush
.1:	rte


;----------------------------------------------------------
xb_ct60_read_temp:
	or.w	#0x0700,sr		; disable interrupts
	moveq.l	#0,d0
	rte


;----------------------------------------------------------
xb_ct60_rwparam:
	or.w	#0x0700,sr		; disable interrupts
	moveq.l	#0,d0
	rte


;----------------------------------------------------------
xb_ct60_vmalloc:
	or.w	#0x0700,sr		    ; disable interrupts
	moveq.l	#0,d0
	rte

	
;----------------------------------------------------------
xb_settime:
	or.w	#0x0700,sr          ; disable interrupts
    move.l  2(a0),d0            ; call our settime
    bsr     xbc_settime
	movea.l	xbios_old(pc),a0    ; call original settime
	jmp		(a0)                ; to update internal tos variables

;----------------------------------------------------------
xb_gettime:
	or.w	#0x0700,sr		    ; disable interrupts
	bsr		xbc_gettime
	rte
	
;----------------------------------------------------------
xb_nvmaccess:
	or.w	#0x0700,sr		    ; disable interrupts
	move.w	2(a0),d0		    ; op
	move.w	4(a0),d1		    ; start
	move.w	6(a0),d2		    ; count
	move.l	8(a0),a0		    ; buffer
	bsr		xbc_nvmaccess
	rte
	

;----------------------------------------------------------
;
; Trap14 xbios handler
;
;----------------------------------------------------------
	DC.B "XBRA"
	DC.B "RAVN"
xbios_old:
	DC.L 0x00B8
xbios_new:
	move	usp,a0
	btst	#5,(sp)			; already super?
	beq.b	.1
	lea		8(sp),a0		; assume longframe
.1:	sub.l	d0,d0
	move.w	(a0),d0			; d0 = opcode

	cmp.w	#0xC600,d0		; 
	bcc		xbios_ct60
	
	;------------------------------------
	; Atari Xbios
	; todo: jumptable these
	;------------------------------------
IFNE XBTIME
	cmp.w	#22,d0			; Settime
	beq		xb_settime
	cmp.w	#23,d0			; Gettime
	beq		xb_gettime
ENDIF

IFNE XBNVM
	cmp.w	#46,d0			; NVMaccess
	beq		xb_nvmaccess
ENDIF

	;------------------------------------
	; Milan / CT60
	;------------------------------------
	cmp.w	#120,d0			; CacheCtrl
	beq 	xb_cache_ctrl
	movea.l	xbios_old(pc),a0
	jmp		(a0)
	

xbios_ct60:
	;------------------------------------
	; CT60
	; todo: jumptable 0xC60? + 0x0C6?
	;------------------------------------
	cmp.w	#0xC60C,d0
	beq 	xb_ct60_cache
	cmp.w	#0xC60D,d0
	beq 	xb_ct60_flush_cache
#if 0
	cmp.w	#0xC60A,d0
	beq 	xb_ct60_read_temp
	cmp.w	#0xC60B,d0
	beq 	xb_ct60_rwparam
	cmp.w	#0xC60E,d0
	beq 	xb_ct60_vmalloc
#endif
	movea.l	xbios_old(pc),a0
	jmp		(a0)
ENDMOD


;----------------------------------------------------------
InstallXbios:
	movem.l	d0/a0,-(sp)			; save registers
	move.w	sr,-(sp)			; disable interrupts
	move.w	#0x2700,sr
	move.l	0x0b8.w,xbios_old	; xbios trap handler
	move.l	#xbios_new,0x0b8.w
	move.w	(sp)+,sr			; restore interrupts
	movem.l	(sp)+,d0/a0			; restore registers
	rts

