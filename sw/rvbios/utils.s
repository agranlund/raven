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
    .EXPORT cacr_set    ; u32 cacr_set(u32)

	.EXPORT cache_flush ; void cache_flush()
    .EXPORT cache_set   ; void cache_set(int16_t enable)

	.EXPORT ticks_get	; u32 ticks_get()

    .EXPORT reset_warm  ; void reset_warm()
    .EXPORT reset_cold  ; void reset_cold()

	.TEXT

ipl_set:
	move.w	sr,d1
	move.w	d1,d2
	and.w	#0xF0FF,d1
	and.w	#0x0F00,d0
	or.w	d0,d1
	move.w	d1,sr
	nop
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
	movec.l cacr,d0
	rts

cacr_set:
    move.w  sr,d1
    or.w    #0x0700,sr
    nop
    cpusha  bc
    nop
    movec.l d0,cacr
    nop
    cpusha  bc
    nop
    move.w  d1,sr
    rts

cache_set:
    cmp.w   #0,d0
    beq.b   cache_off
cache_on:
    move.l  #0xA0C08000,d0
    bra.s   cacr_set
cache_off:
    moveq.l #0,d0
    bra.s   cacr_set

cache_flush:
	move.w	sr,d0
	or.w	#0x0700,sr
	nop
	cpusha	bc
	nop
	move.w  d0,sr
	rts

ticks_get:
	move.l	0x4ba.w,d0
	rts

reset_warm:
    nop
    move.w  #0x2700,sr
    nop
    jmp     0x00e00000
    
reset_cold:
    nop
    move.w  #0x2700,sr
    nop
    move.l  0x40000004,a0
    jmp     (a0)
