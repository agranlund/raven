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

KEYREPEAT_FIX EQU 1         ; prevent ps/2 hardware repeat

	.XREF 	Setcookie

	.EXPORT InstallEiffel

	.BSS

IFNE KEYREPEAT_FIX
eiffel_ignore:  ds.w    1
ENDIF
eiffel_data:	ds.b	2
eiffel_temp:	ds.b	6       ; eiffel temperature (board)
eiffel_temp2:   ds.b    6       ; ckbd temperature (cpu)
eiffel_info:    ds.b    6       ; ckbd version

	.TEXT

;----------------------------------------------------------
;
; Eiffel kbvec
;
;----------------------------------------------------------
IFNE KEYREPEAT_FIX
    DC.B "XBRA"
    DC.B "RAVN"
kbvec_old:
    DC.L 0
kbvec_new:
    cmp.w   eiffel_ignore,d0
    beq.b  .1
    move.w  d0,eiffel_ignore
    move.l  kbvec_old,-(sp)		; old vec
.1: rts
ENDIF

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
    ; prepare jump to old statvec handler
    move.l	statvec_old,-(sp)

    ; eiffel temperature message (board)
	cmp.b	#0x03,(a0)
    bne.b   .1
    move.l  1(a0),eiffel_temp+0
    move.w  5(a0),eiffel_temp+0+4
    rts
    ; ckbd temperature message (cpu)
.1: cmp.b   #0x2A,(a0)
    bne.b   .2
    move.l  1(a0),eiffel_temp+6
    move.w  5(a0),eiffel_temp+6+4
    rts
    ; ckbd version info
.2: cmp.b   #0x2C,(a0)
    bne.b   .3
    move.l  1(a0),eiffel_temp+12
    move.w  5(a0),eiffel_temp+12+4
.3: rts


;----------------------------------------------------------
InstallEiffel:
	movem.l	d0-d1/a0,-(sp)		; save registers
	move.w	sr,-(sp)			; disable interrupts
	move.w	#0x2700,sr

	; Eiffel vectors
	move.w	#34,-(sp)			; Kbdvbase()
	trap	#14
	addq.l	#2,sp
	move.l	d0,a0
.1:	move.b	36(a0),d0			; wait for ready
	bne.b	.1

IFNE KEYREPEAT_FIX
    move.w  #0,eiffel_ignore
	move.l	-4(a0),d0			; replace kbvec
	move.l	d0,kbvec_old
	move.l	#kbvec_new,-4(a0)
ENDIF

	move.l	#0,eiffel_data+0
	move.l	#0,eiffel_data+4
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

