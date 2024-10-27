;-------------------------------------------------------------------------------
; Raven hardware utilities
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

	.EXPORT ipl_set		; u16 ipl_set(u16)
	.EXPORT pcr_get		; u32 pcr_get()
	.EXPORT vbr_get		; u32 vbr_get()
	.EXPORT cacr_get	; u32 cacr_get()
	.EXPORT cache_clear	; void cache_clear()
	.EXPORT ticks_get	; u32 ticks_get()

	.TEXT

ipl_set:
	move.w	sr,d1
	move.w	d1,d2
	and.w	#0xF0FF,d1
	and.w	#0x0F00,d0
	or.w	d0,d1
	move.w	d1,sr
	move.w	d2,d0
	and.w	#0x0F00,d0
	rts

pcr_get:
	dc.l	0x4e7a0808			; movec pcr,d0
	rts
	

vbr_get:
	dc.l	0x4e7a0801			; movec vbr,d0
	rts

cacr_get:
	movec	cacr,d0
	rts

ticks_get:
	move.l	0x4ba.w,d0
	rts

cache_clear:
	nop
	cpusha	bc
	nop
	rts
