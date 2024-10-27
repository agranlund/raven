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
	
	.BSS
sp060_old_2c:	.ds.l	1	; fpu unimplemented instruction
sp060_old_c4:	.ds.l	1	; fpu inexact
sp060_old_c8:	.ds.l	1	; fpu divzero
sp060_old_cc:	.ds.l	1	; fpu underflow
sp060_old_d0:	.ds.l	1	; fpu operand error
sp060_old_d4:	.ds.l	1	; fpu overflow
sp060_old_d8:	.ds.l	1	; fpu snan
sp060_old_dc:	.ds.l	1	; fpu unimplemented data type
sp060_old_f0:	.ds.l	1	; fpu unimplemented effective address
sp060_old_f4:	.ds.l	1	; cpu unimplemented instruction
	

	.TEXT

;---------------------------------------------------------
Install060sp:
	movem.l	d0/a0,-(sp)

	; save old handlers
	move.l	0x2c,sp060_old_2c
	move.l	0xc4,sp060_old_c4
	move.l	0xc8,sp060_old_c8
	move.l	0xcc,sp060_old_cc
	move.l	0xd0,sp060_old_d0
	move.l	0xd4,sp060_old_d4
	move.l	0xd8,sp060_old_d8
	move.l	0xdc,sp060_old_dc
	move.l	0xf0,sp060_old_f0
	move.l	0xf4,sp060_old_f4

	; integer instructions
	lea		sp060_isp_table+0x80(pc),a0
	move.l	a0,0xf4
	
	; floating point instructions
	.dc.w	0x4e7a,0x0808	; movec.l pcr,d0
	btst	#16,d0			; EC/LC cpu?
	bne.s	.1
	lea.l	sp060_fsp_table+0x80(pc),a0
	move.l	a0,0xd8
	addq.l	#8,a0
	move.l	a0,0xd0
	addq.l	#8,a0
	move.l	a0,0xd4
	addq.l	#8,a0
	move.l	a0,0xcc
	addq.l	#8,a0
	move.l	a0,0xc8
	addq.l	#8,a0
	move.l	a0,0xc4
	addq.l	#8,a0
	move.l	a0,0x2c
	addq.l	#8,a0
	move.l	a0,0xdc
	addq.l	#8,a0
	move.l	a0,0xf0

.1:	movem.l	(sp)+,d0/a0
	rts

;---------------------------------------------------------
;
;---------------------------------------------------------


sp060_mem_copy1:
	move.b 	(a0)+,(a1)+
sp060_mem_copy:
	dbf 	d0,sp060_mem_copy1
	clr.l 	d1
	rts

sp060_mem_read_byte:
	clr.l 	d0
	move.b 	(a0),d0
	clr.l 	d1
	rts
	
sp060_mem_read_word:
	clr.l	d0
	
sp060_mem_read_word1:
	move.w	(a0),d0
	clr.l 	d1
	rts
		
sp060_mem_read_long:
	move.l 	(a0),d0
	clr.l 	d1
	rts

sp060_mem_write_byte:
	move.b	d0,(a0)
	clr.l 	d1
	rts

sp060_mem_write_word:
	move.w 	d0,(a0)
	clr.l 	d1
	rts
		
sp060_mem_write_long:
	move.l	d0,(a0)
	clr.l 	d1
	rts

sp060_real_chk:
sp060_real_divbyzero:
	tst.b	(sp)			; trace enabled?
	bpl.b	sp060_rte
	move.b	#0x24,0x07(sp)	; set trace vec
	bra.b	sp060_real_trace

sp060_real_trace:
	move.l	0x24,-(sp)
	rts
	
sp060_real_access:
	move.l	0x08,-(sp)
	rts

sp060_real_cas:
	bra.l 	sp060_isp_table+0x80+0x08

sp060_real_cas2:
	bra.l 	sp060_isp_table+0x80+0x10


sp060_real_trap:
sp060_isp_done:
sp060_fpsp_done:
sp060_rte:
	rte


sp060_real_lock_page:
sp060_real_unlock_page:
	clr.l	d0
	rts


sp060_real_fline:
	move.l		sp060_old_2c,-(sp)
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
	move.l		d0,-(sp)
	.dc.w		0x4e7a,0x0808	; movec.l	pcr,d0
	bclr 		#1,d0
	.dc.w		0x4e7b,0x0808	; movec.l	d0,pcr
	move.l 		(sp)+,d0
	move.l 		12(sp),2(sp)
	fmovem.l	#0,fpcr
	rte

sp060_real_snan:
	fsave 		-(sp)
	move.w 		#0x6000,2(sp)
	frestore 	(sp)+
	fmovem.l 	#0,fpcr
	move.l		sp060_old_d8,-(sp)
	rts

sp060_real_operr:
	fsave 		-(sp)
	move.w 		#0x6000,2(sp)
	frestore 	(sp)+
	fmovem.l 	#0,fpcr
	move.l		sp060_old_d0,-(sp)
	rts
	
sp060_real_ovfl:
	fsave 		-(sp)
	move.w 		#0x6000,2(sp)
	frestore 	(sp)+
	fmovem.l 	#0,fpcr
	move.l		sp060_old_d4,-(sp)
	rts
	
sp060_real_unfl:
	fsave 		-(sp)
	move.w 		#0x6000,2(sp)
	frestore 	(sp)+
	fmovem.l 	#0,fpcr
	move.l		sp060_old_cc,-(sp)
	rts

sp060_real_dz:
	fsave 		-(sp)
	move.w		#0x6000,2(sp)
	frestore 	(sp)+
	fmovem.l 	#0,fpcr
	move.l		sp060_old_c8,-(sp)
	rts

sp060_real_inex:
	fsave 		-(sp)
	move.w 		#0x6000,2(sp)
	frestore 	(sp)+
	fmovem.l 	#0,fpcr
	move.l		sp060_old_c4,-(sp)
	rts
	


;---------------------------------------------------------
;
;---------------------------------------------------------
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
	.dc.l sp060_mem_copy-sp060_isp_table
	.dc.l sp060_mem_copy-sp060_isp_table
	.dc.l sp060_mem_copy-sp060_isp_table
	.dc.l sp060_mem_read_word1-sp060_isp_table
	.dc.l sp060_mem_read_long-sp060_isp_table
	.dc.l sp060_mem_read_byte-sp060_isp_table
	.dc.l sp060_mem_read_word-sp060_isp_table
	.dc.l sp060_mem_read_long-sp060_isp_table
	.dc.l sp060_mem_write_byte-sp060_isp_table
	.dc.l sp060_mem_write_word-sp060_isp_table
	.dc.l sp060_mem_write_long-sp060_isp_table
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	include "motorola\isp.sa"

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
	.dc.l sp060_mem_copy-sp060_fsp_table
	.dc.l sp060_mem_copy-sp060_fsp_table
	.dc.l sp060_mem_copy-sp060_fsp_table
	.dc.l sp060_mem_read_word1-sp060_fsp_table
	.dc.l sp060_mem_read_long-sp060_fsp_table
	.dc.l sp060_mem_read_byte-sp060_fsp_table
	.dc.l sp060_mem_read_word-sp060_fsp_table
	.dc.l sp060_mem_read_long-sp060_fsp_table
	.dc.l sp060_mem_write_byte-sp060_fsp_table
	.dc.l sp060_mem_write_word-sp060_fsp_table
	.dc.l sp060_mem_write_long-sp060_fsp_table
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	.dc.l 0
	include "motorola\fpsp.sa"

