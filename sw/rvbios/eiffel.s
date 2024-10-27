;-------------------------------------------------------------------------------
; Raven eiffel extensions
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

	.XREF 	Setcookie

	.EXPORT InstallEiffel

	.BSS

eiffel_data:	ds.b	2
eiffel_temp:	ds.b	6

	.TEXT

;----------------------------------------------------------
;
; Eiffel statvec
;
;----------------------------------------------------------
	DC.B "XBRA"
	DC.B "RAVN"
statvec_old:
	DC.L 0
statvec_new:
	cmp.b	#3,(a0)
	bne.s	.1
	movem.l	a0-a1,-(sp)
	move.l	#eiffel_temp,a1
	addq.l	#1,a0
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	move.b	(a0)+,(a1)+
	movem.l	(sp)+,a0-a1
.1:	move.l	statvec_old,-(sp)	; old vec
	rts



;----------------------------------------------------------
InstallEiffel:
	movem.l	d0-d1/a0,-(sp)		; save registers
	move.w	sr,-(sp)			; disable interrupts
	move.w	#0x2700,sr

	; Eiffel statvec
	move.l	#0,eiffel_data+0
	move.l	#0,eiffel_data+4
	move.w	#34,-(sp)			; Kbdvbase()
	trap	#14
	addq.l	#2,sp
	move.l	d0,a0
.1:	move.b	36(a0),d0			; wait for ready
	bne.b	.1
	move.l	12(a0),d0			; replace statvec
	move.l	d0,statvec_old
	move.l	#statvec_new,12(a0)

	move.l	#0x54656D70,d0		; cookie id:   'Temp'
	move.l	#eiffel_temp,d1		; cookie data: &eiffel_temp
	jsr		Setcookie

	move.l	#0x45696666,d0		; cookie id:   'Eiff'
	move.l	#eiffel_data,d1		; cookie data: &eiffel_data
	jsr		Setcookie

	move.w	(sp)+,sr			; restore interrupts
	movem.l	(sp)+,d0-d1/a0		; restore registers
	rts

