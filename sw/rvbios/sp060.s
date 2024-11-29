;-------------------------------------------------------------------------------
; 68060 cpu support package
; (c)2024 Anders Granlund
; 
; Derived from CT60 xbios (c) Didier Mequignon 
; M68060 Software Package (c) Motorola Inc.
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


;
; todo, freemint implementation differences
;	- separate super/user memaccess functions
;	- fpu exceptions just rte instead of calling old vector
;     (sp060_real_snan and friends)
;


	.EXPORT Install060sp
	
	.TEXT



;---------------------------------------------------------
; install
;---------------------------------------------------------
Install060sp:
	movem.l	d0/a0,-(sp)

	; integer instructions

	lea		xbf4(pc),a0			; unsupported integer instruction
	move.l	0xf4,-4(a0)
	move.l	a0,0xf4
	
	; floating point instructions
	dc.w	0x4e7a,0x0808	; movec.l pcr,d0
	btst	#16,d0			; EC/LC cpu?
	bne		.1

	lea		xbd8(pc),a0		; snan
	move.l	0xd8,-4(a0)
	move.l	a0,0xd8
	lea		xbd0(pc),a0		; operr
	move.l	0xd0,-4(a0)
	move.l	a0,0xd4
	lea		xbd4(pc),a0		; overflow
	move.l	0xd4,-4(a0)
	move.l	a0,0xd4
	lea		xbcc(pc),a0		; underflow
	move.l	0xcc,-4(a0)
	move.l	a0,0xcc
	lea		xbc8(pc),a0		; divide by zero
	move.l	0xc8,-4(a0)
	move.l	a0,0xc8
	lea		xbc4(pc),a0		; inexact result
	move.l	0xc4,-4(a0)
	move.l	a0,0xc4
	lea		xb2c(pc),a0		; line-f
	move.l	0x2c,-4(a0)
	move.l	a0,0x2c
	lea		xbdc(pc),a0		; unsupported fpu instruction
	move.l	0xdc,-4(a0)
	move.l	a0,0xdc
	lea		xbf0(pc),a0		; unsupported effective address
	move.l	0xf0,-4(a0)
	move.l	a0,0xf0

	fmovem.l	#0,fpcr

.1:	movem.l	(sp)+,d0/a0
	rts



;---------------------------------------------------------
; xbra
;---------------------------------------------------------
align 4
dc.l	0x58425241, 0x46505350, 0
xbf4:	bra	sp060_isp_table+0x80+0x00
align 4
dc.l	0x58425241, 0x46505350, 0
xbd8:	bra	sp060_fsp_table+0x80+0x00
align 4
dc.l	0x58425241, 0x46505350, 0
xbd0:	bra	sp060_fsp_table+0x80+0x08
align 4
dc.l	0x58425241, 0x46505350, 0
xbd4:	bra	sp060_fsp_table+0x80+0x10
align 4
dc.l	0x58425241, 0x46505350, 0
xbcc:	bra	sp060_fsp_table+0x80+0x18
align 4
dc.l	0x58425241, 0x46505350, 0
xbc8:	bra	sp060_fsp_table+0x80+0x20
align 4
dc.l	0x58425241, 0x46505350, 0
xbc4:	bra	sp060_fsp_table+0x80+0x28
align 4
dc.l	0x58425241, 0x46505350, 0
xb2c:	bra	sp060_fsp_table+0x80+0x30
align 4
dc.l	0x58425241, 0x46505350, 0
xbdc:	bra	sp060_fsp_table+0x80+0x38
align 4
dc.l	0x58425241, 0x46505350, 0
xbf0:	bra	sp060_fsp_table+0x80+0x40

;---------------------------------------------------------
;
;---------------------------------------------------------

sp060_real_chk:
sp060_real_divbyzero:
	tst.b	(sp)			; trace enabled?
	bpl.b	sp060_rte
	move.b	#0x24,0x07(sp)	; set trace vec
sp060_real_trace:
	move.l	0x24,-(sp)
	rts

sp060_real_trap:
sp060_isp_done:
sp060_fpsp_done:
sp060_rte:
	rte	

sp060_real_access:
	move.l	0x08,-(sp)
	rts

sp060_real_cas:
	bra.l 	sp060_isp_table+0x80+0x08

sp060_real_cas2:
	bra.l 	sp060_isp_table+0x80+0x10

sp060_real_lock_page:
sp060_real_unlock_page:
	clr.l	d0
	rts

sp060_real_fline:
	move.l		xb2c-4(pc),-(sp)
	rts

sp060_real_bsun:
	fsave 		-(sp)
	fmovem.l 	fpsr,-(sp)
	andi.b 		#0xfe,(sp)
	fmovem.l 	(sp)+,fpsr
	lea 		12(sp),sp
	fmovem.l 	#0,fpcr
	rte

sp060_real_fpu_disabled:
	move.l		d0,-(sp)			; save regs
	dc.w		0x4e7a,0x0808		; movec.l	pcr,d0
	bclr 		#1,d0				; enable fpu
	dc.w		0x4e7b,0x0808		; movec.l	d0,pcr
	move.l 		(sp)+,d0			; restore regs
	move.l 		12(sp),2(sp)
	fmovem.l	#0,fpcr
	rte


IF 1

sp060_real_ovfl:
sp060_real_unfl:
sp060_real_operr:
sp060_real_snan:
sp060_real_dz:
sp060_real_inex:
	fsave		-(sp)
	move.w		#0x6000,2(sp)
	frestore	(sp)+
	fmovem.l	#0,fpcr
	rte

ELSE


sp060_real_ovfl:
	move.l	xbd4-4(pc),-(sp)
	bra.b	sp060_clear_fpu_exception_and_go

sp060_real_unfl:
	move.l	xbcc-4(pc),-(sp)
	bra.b	sp060_clear_fpu_exception_and_go

sp060_real_operr:
	move.l	xbd0-4(pc),-(sp)
	bra.b	sp060_clear_fpu_exception_and_go

sp060_real_snan:
	move.l	xbd8-4(pc),-(sp)
	bra.b	sp060_clear_fpu_exception_and_go

sp060_real_dz:
	move.l	xbc8-4(pc),-(sp)
	bra.b	sp060_clear_fpu_exception_and_go

sp060_real_inex:
	move.l	xbc4-4(pc),-(sp)
	bra.b	sp060_clear_fpu_exception_and_go

sp060_clear_fpu_exception_and_go:
	fsave		-(sp)
	move.w		#0x6000,2(sp)
	frestore	(sp)+
	fmovem.l	#0,fpcr
	rts

ENDIF


;---------------------------------------------------------
; memory access functions
; todo:
;
; - Do we really need userspace read/write?
;   Looks like ct60 doesn't care
;   If we do, might be a good idea to move sfc/dfc setter
;   at vector entry to avoid having to do it all the time
; - Copy routine could be optimized
;---------------------------------------------------------
sp060_imem_read:
sp060_dmem_read:
	subq.l	#1,d0			; d0 = count
	btst	#5,4(a6)		; super?
	bne.s	.2
	moveq.l	#1,d1			; user read
	movec	d1,sfc			; source = userspace
.1:	moves.b	(a0)+,d1
	move.b	d1,(a1)+
	dbra	d0,.1
	bra.b	.3
.2:	move.b	(a0)+,(a1)+		; super read
	dbra	d0,.2
.3:	clr.l	d1
	rts

sp060_dmem_write:
	subq.l	#1,d0			; d0 = count
	btst	#5,4(a6)		; super?
	bne.s	.2
	moveq.l	#1,d1			; user write
	movec	d1,dfc			; dest = userspace
.1:	move.b	(a0)+,d1
	moves.b	d1,(a1)+
	dbra	d0,.1
	bra.b	.3
.2:	move.b	(a0)+,(a1)+		; super write
	dbra	d0,.2
.3:	clr.l	d1
	rts

sp060_dmem_read_byte:
	clr.l	d0
	btst	#5,4(a6)
	bne.b	.2
	moveq.l	#1,d1
	movec	d1,sfc
	moves.b	(a0),d0
	bra.b	.3
.2:	move.b	(a0),d0
.3:	clr.l	d1
	rts

sp060_dmem_read_word:
sp060_imem_read_word:
	clr.l	d0
	btst	#5,4(a6)
	bne.b	.2
	moveq.l	#1,d1
	movec	d1,sfc
	moves.w	(a0),d0
	bra.b	.3
.2:	move.w	(a0),d0
.3:	clr.l	d1
	rts

sp060_dmem_read_long:
sp060_imem_read_long:
	btst	#5,4(a6)
	bne.b	.2
	moveq.l	#1,d1
	movec	d1,sfc
	moves.l	(a0),d0
	bra.b	.3
.2:	move.l	(a0),d0
.3:	clr.l	d1
	rts

sp060_dmem_write_byte:
	btst	#5,4(a6)
	bne.b	.2
	moveq.l	#1,d1
	movec	d1,dfc
	moves.b	d0,(a0)
	bra.b	.3
.2:	move.b	d0,(a0)
.3:	clr.l	d1
	rts

sp060_dmem_write_word:
	btst	#5,4(a6)
	bne.b	.2
	moveq.l	#1,d1
	movec	d1,dfc
	moves.w	d0,(a0)
	bra.b	.3
.2:	move.w	d0,(a0)
.3:	clr.l	d1
	rts

sp060_dmem_write_long:
	btst	#5,4(a6)
	bne.b	.2
	moveq.l	#1,d1
	movec	d1,dfc
	moves.l	d0,(a0)
	bra.b	.3
.2:	move.l	d0,(a0)
.3:	clr.l	d1
	rts


;---------------------------------------------------------
;
;---------------------------------------------------------
align 16
sp060_isp_table:
	.dc.l sp060_real_chk-sp060_isp_table		; or 0xF4
	.dc.l sp060_real_divbyzero-sp060_isp_table	; or 0xF4
	.dc.l sp060_real_trace-sp060_isp_table
	.dc.l sp060_real_access-sp060_isp_table
	.dc.l sp060_isp_done-sp060_isp_table
	.dc.l sp060_real_cas-sp060_isp_table
	.dc.l sp060_real_cas2-sp060_isp_table
	.dc.l sp060_real_lock_page-sp060_isp_table
	.dc.l sp060_real_unlock_page-sp060_isp_table
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l sp060_imem_read-sp060_isp_table
	.dc.l sp060_dmem_read-sp060_isp_table
	.dc.l sp060_dmem_write-sp060_isp_table
	.dc.l sp060_imem_read_word-sp060_isp_table
	.dc.l sp060_imem_read_long-sp060_isp_table
	.dc.l sp060_dmem_read_byte-sp060_isp_table
	.dc.l sp060_dmem_read_word-sp060_isp_table
	.dc.l sp060_dmem_read_long-sp060_isp_table
	.dc.l sp060_dmem_write_byte-sp060_isp_table
	.dc.l sp060_dmem_write_word-sp060_isp_table
	.dc.l sp060_dmem_write_long-sp060_isp_table
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	include "motorola\isp.sa"

align 16
sp060_fsp_table:
	.dc.l sp060_real_bsun-sp060_fsp_table
	.dc.l sp060_real_snan-sp060_fsp_table
	.dc.l sp060_real_operr-sp060_fsp_table
	.dc.l sp060_real_ovfl-sp060_fsp_table
	.dc.l sp060_real_unfl-sp060_fsp_table
	.dc.l sp060_real_dz-sp060_fsp_table
	.dc.l sp060_real_inex-sp060_fsp_table
	.dc.l sp060_real_fline-sp060_fsp_table 		; or 0x2C
	.dc.l sp060_real_fpu_disabled-sp060_fsp_table
	.dc.l sp060_real_trap-sp060_fsp_table
	.dc.l sp060_real_trace-sp060_fsp_table
	.dc.l sp060_real_access-sp060_fsp_table
	.dc.l sp060_fpsp_done-sp060_fsp_table
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l sp060_imem_read-sp060_fsp_table
	.dc.l sp060_dmem_read-sp060_fsp_table
	.dc.l sp060_dmem_write-sp060_fsp_table
	.dc.l sp060_imem_read_word-sp060_fsp_table
	.dc.l sp060_imem_read_long-sp060_fsp_table
	.dc.l sp060_dmem_read_byte-sp060_fsp_table
	.dc.l sp060_dmem_read_word-sp060_fsp_table
	.dc.l sp060_dmem_read_long-sp060_fsp_table
	.dc.l sp060_dmem_write_byte-sp060_fsp_table
	.dc.l sp060_dmem_write_word-sp060_fsp_table
	.dc.l sp060_dmem_write_long-sp060_fsp_table
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	include "motorola\fpsp.sa"

