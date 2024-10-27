;-------------------------------------------------------------------------------
; rom loader
; (c) 2024 Anders Granlund
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

	.GLOBL	errno
	.GLOBL	_AtExitVec,_FilSysVec
	
	.XREF	rom_main

	.TEXT

; purec header
;	-38+0	int		0x601b (bra.s rom_header)
;	-38+2	long	tsize
;	-38+6	long	dsize
;	-38+10	long	bsize
;	-38+14	long	ssize
;	-38+18	long	?
;	-38+22	long	?
;	-38+26	long	tbase
;	-38+30	long	dbase
;	-38+34	long	bbase

rom_header:

	bra.b	rom_entry
	.dc.b	"RVXB"

rom_entry:

	; copy text + data
	lea.l	rom_header(pc),a0		; a0 = src text+data
	move.l	rom_header-38+26(pc),a1	; a1 = dst text+data
	move.l	rom_header-38+2(pc),d0	; d0 = text size
	add.l	rom_header-38+6(pc),d0	;    + data size
	lsr.l	#2,d0
.1:	move.l	(a0)+,(a1)+
	dbra	d0,.1

	; clear bss
	move.l	rom_header-38+34(pc),a1	; a1 = dst bss	
	move.l	rom_header-38+10(pc),d0	; d0 = bss size
	lsr.l	#2,d0
.2:	move.l	#0,(a1)+
	dbra	d0,.2

	; clear cache
	nop
	cpusha	bc
	nop

	; run
	jmp	rom_main



	.BSS

__bss:
_base:
_BasPag:        ds.l    1
_app:           ds.w    1
_StkLim:        ds.l    1
_PgmSize:       ds.l    1
_RedirTab:      ds.l    6

	.DATA
                
__data:
errno:          dc.w    0
_AtExitVec:     dc.l    0
_FilSysVec:     dc.l    0