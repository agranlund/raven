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



;-------------------------------------------------------------------------------
;	TOS1
;		0x0016	void SetTime(u32 time)
;		0x0017	u32  GetTime(void)
;
;		0x001C	u8 Giaccess(u16 data, u16 regno)
;		0x001D	void Offgibit(u16 bitno)
;		0x001E	void Ongibit(u16 bitno)
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
    .XREF   cache_set
    .XREF   cache_flush

	.XREF 	xbc_settime
	.XREF 	xbc_gettime
	.XREF 	xbc_nvmaccess
    .XREF   xbc_puntaes
    .XREF   xbc_read_temp

	.XREF	xbtable

	.EXPORT InstallTrap14

	.TEXT


;----------------------------------------------------------
;
; Trap14 xbios handler
;
;----------------------------------------------------------
InstallTrap14:
	movem.l	d0/a0,-(sp)			; save registers
	move.w	sr,-(sp)			; disable interrupts
	move.w	#0x2700,sr
	move.l	0x0b8.w,xbios_old	; xbios trap handler
	move.l	#xbios_new,0x0b8.w
	move.w	(sp)+,sr			; restore interrupts
	movem.l	(sp)+,d0/a0			; restore registers
	rts

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
.1:	moveq	#0,d0
	move.w	(a0),d0			; d0 = opcode

	;------------------------------------
	; Atari
	;------------------------------------
	cmp.w	#256,d0
	bcc.b	.2
	move.l	(xbtable, d0.w*4),d0
	beq.b	.3
	move.l	d0,a1
	jmp		(a1)

	;------------------------------------
	; CT60
	;------------------------------------
.2:	cmp.w	#0x0C6A,d0
	bcc		xb_ct60

	;------------------------------------
	; Original
	;------------------------------------
.3:	movea.l	xbios_old(pc),a0
	jmp		(a0)


;----------------------------------------------------------
xb_settime:
    move.w  sr,-(sp)
	or.w	#0x0700,sr          ; disable interrupts
    move.l  2(a0),d0            ; call our settime
    bsr     xbc_settime
	movea.l	xbios_old(pc),a0    ; call original settime
    move.w  (sp)+,sr
	jmp		(a0)                ; to update internal tos variables

;----------------------------------------------------------
xb_gettime:
	or.w	#0x0700,sr		    ; disable interrupts
	bsr		xbc_gettime
	rte

;----------------------------------------------------------
xb_giaccess:
	or.w	#0x0700,sr		    ; disable interrupts
	move.w	4(a0),d0			; regno
	move.b	d0,d1
	and.b	#0xf,d1
	move.b	d1,0xff8800
	btst.b	#7,d0				; (regno & 0x80) means write
	beq.b	.2
	move.w	2(a0),d0			; data
	cmp.b	#14,d1
	bne.b	.1
	eor.b	#0x40,d0			; portA = invert bit6
.1:	move.b	d0,0xff8802
.2: moveq	#0,d0
	move.b	0xff8800,d0
	cmp.b	#14,d1
	bne.b	.3
	eor.b	#0x40,d0			; portA = invert bit6
.3: rte

;----------------------------------------------------------
xb_offgibit:
	or.w	#0x0700,sr		    ; disable interrupts
	move.w	2(a0),d1
	move.b	d1,d2				; d1 = regular bits
	eor.b	#0x40,d2			; d2 = bit6
	and.b	#0x40,d2
	move.b	#14,0xff8800
	move.b	0xff8800,d0
	and.b	d1,d0				; regular offgibit logic
	or.b	d2,d0				; bit6 off = force 1
	move.b	d0,0xff8802
	rte

;----------------------------------------------------------
xb_ongibit:
	or.w	#0x0700,sr		    ; disable interrupts
	move.w	2(a0),d1
	move.b	d1,d2
	and.b	#0xbf,d1			; d1 = regular bits
	eor.b	#0x40,d2			; d2 = bit6
	or.b	#0xbf,d2
	move.b	#14,0xff8800		; port a
	move.b	0xff8800,d0
	and.b	d2,d0				; bit6 on = force 0
	or.b	d1,d0				; regular ongibit logic
	move.b	d0,0xff8802
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
xb_puntaes:
    addq.l  #2,a0
    bsr     xbc_puntaes
	movea.l	xbios_old(pc),a0    ; call original
	jmp		(a0)                ; to update internal tos variables

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
    bsr     cache_flush
	moveq.l	#0,d0
	rte
.2:	cmp.w	#2,d0			; flush instruction cache
	bne.b	.3
    bsr     cache_flush
	moveq.l	#0,d0
	rte
.3:	cmp.w	#3,d0			; flush both caches
	bne.b	.4
    bsr     cache_flush
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
    move.w  d1,d0
    bsr     cache_set
    moveq.l #0,d0
    rte
.6:	cmp.w	#6,d0			; inquire instruction cache mode
	bne.b	.7
	movec.l	cacr,d0
	rol.w	#1,d0
	and.l	#1,d0
	rte
.7:	cmp.w	#7,d0			; activate/deactivate instruction cache
	bne.b	.8
    move.w  d1,d0
    bsr     cache_set
    moveq.l #0,d0
    rte
.8:	move.l	#-5,d0			; EBADRQ
	rte


;----------------------------------------------------------
xb_ct60:
	cmp.w   #0xC600,d0
	bcc.s   xb_ct60new

	cmp.w   #0x0C6A,d0
	beq     xb_ct60_read_temp
;	cmp.w	#0x0C6B,d0
;	beq 	xb_ct60_rwparam
	cmp.w   #0x0C6C,d0
	beq 	xb_ct60_cache
	cmp.w   #0x0C6D,d0
	beq 	xb_ct60_flush_cache
;	cmp.w	#0x0C6E,d0
;	beq 	xb_ct60_vmalloc
	movea.l	xbios_old(pc),a0
	jmp		(a0)

xb_ct60new:
	cmp.w	#0xC60A,d0
	beq 	xb_ct60_read_temp
;	cmp.w	#0xC60B,d0
;	beq 	xb_ct60_rwparam
	cmp.w	#0xC60C,d0
	beq 	xb_ct60_cache
	cmp.w	#0xC60D,d0
	beq 	xb_ct60_flush_cache
;	cmp.w	#0xC60E,d0
;	beq 	xb_ct60_vmalloc
	movea.l	xbios_old(pc),a0
	jmp		(a0)

;----------------------------------------------------------
xb_ct60_cache:
	move.w	2(a0),d0
    bmi.b   .1
    bsr     cache_set
.1: movec.l cacr,d0
	rte

;----------------------------------------------------------
xb_ct60_flush_cache:
    bsr     cache_flush
    moveq.l #0,d0
    rte

;----------------------------------------------------------
xb_ct60_read_temp:
    bsr     xbc_read_temp
	rte

;----------------------------------------------------------
xb_ct60_rwparam:
	moveq.l	#0,d0
	rte

;----------------------------------------------------------
xb_ct60_vmalloc:
	moveq.l	#0,d0
	rte

	

xbtable:
	dc.l	0,0,0,0,0,0,0,0,0,0	; 0
	dc.l	0,0,0,0,0,0,0,0,0,0	; 10
IFNE XBTIME
	dc.l	0,0					; 20
	dc.l	xb_settime			; 22
	dc.l	xb_gettime			; 23
	dc.l	0,0,0,0				; 24
ELSE
	dc.l	0,0,0,0,0,0,0,0
ENDIF
	dc.l	xb_giaccess			; 28
	dc.l	xb_offgibit			; 29
	dc.l	xb_ongibit			; 30

	dc.l	0,0,0,0,0,0,0,0		; 31
	dc.l	xb_puntaes			; 39
IFNE XBNVM
	dc.l	0,0,0,0,0,0			; 40
	dc.l	xb_nvmaccess		; 46
	dc.l	0,0,0				; 47
ELSE
	dc.l	0,0,0,0,0,0,0,0,0,0
ENDIF
	dc.l	0,0,0,0,0,0,0,0,0,0	; 50
	dc.l	0,0,0,0,0,0,0,0,0,0	; 60
	dc.l	0,0,0,0,0,0,0,0,0,0	; 70
	dc.l	0,0,0,0,0,0,0,0,0,0	; 80
	dc.l	0,0,0,0,0,0,0,0,0,0	; 90
	dc.l	0,0,0,0,0,0,0,0,0,0	; 100
	dc.l	0,0,0,0,0,0,0,0,0,0	; 110
	dc.l	0,0,0,0,0,0,0,0,0,0	; 120
	dc.l	0,0,0,0,0,0,0,0,0,0 ; 130
	dc.l	0,0,0,0,0,0,0,0,0,0 ; 140
	dc.l	0,0,0,0,0,0,0,0,0,0	; 150
	dc.l	xb_cache_ctrl		; 160
	dc.l	0,0,0,0,0,0,0,0,0	; 161
	dc.l	0,0,0,0,0,0,0,0,0,0	; 170
	dc.l	0,0,0,0,0,0,0,0,0,0	; 180
	dc.l	0,0,0,0,0,0,0,0,0,0	; 190
	dc.l	0,0,0,0,0,0,0,0,0,0	; 200
	dc.l	0,0,0,0,0,0,0,0,0,0	; 210
	dc.l	0,0,0,0,0,0,0,0,0,0	; 220
	dc.l	0,0,0,0,0,0,0,0,0,0	; 230
	dc.l	0,0,0,0,0,0,0,0,0,0	; 240
	dc.l	0,0,0,0,0,0			; 250
	