;-------------------------------------------------------------------------------
; MPU401 uart mode driver for Atari bios/xbios
; (c)2024 Anders Granlund
;-------------------------------------------------------------------------------
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
;-------------------------------------------------------------------------------
    .text
    .export xbios_old
    .export xbios_new

    .export new_bconout3
    .export new_bcostat3
    .export new_bconin3
    .export new_bconstat3

    .xref midi_bconout
    .xref midi_bcostat

;----------------------------------------------------------
; void bconout3(int16_t dev, int16_t chr)
; returns aaaaaabb where a is timestamp and b is data
; dev = 4(a7)
; chr = 6(a7)
;----------------------------------------------------------
new_bconout3:
    movem.l d0-d2/a0-a2,-(sp)
    move.w  30(a7),d0
    bsr     midi_bconout
    movem.l (sp)+,d0-d2/a0-a2
    rts

;----------------------------------------------------------
; int32_t midiost()
; return -1 = ok to send, 0 = not ready
;----------------------------------------------------------
new_bcostat3:
    movem.l d1-d2/a0-a2,-(sp)
    bsr     midi_bcostat
    movem.l (sp)+,d1-d2/a0-a2
    rts

;----------------------------------------------------------
; Xbios, opcode #12
; void Midiws(int16_t cnt, uint8_t* ptr)
;   cnt.w = 0(a0)
;   ptr.l = 2(a0)
;----------------------------------------------------------
xbios_midiws:
    movem.l d2-d3/a2-a3,-(sp)
    moveq.l #0,d3
    move.w  0(a0),d3
    movea.l 2(a0),a3
.1: move.b  (a3)+,d0
    bsr     midi_bconout
    dbf     d3,.1
    movem.l (sp)+,d2-d3/a2-a3
    rte


;----------------------------------------------------------
;
; Trap14 xbios handler
;
;----------------------------------------------------------
	dc.b "XBRA"
	dc.b "ISAM"
xbios_old:
	dc.l 0x00B8
xbios_new:
	move	usp,a0
	btst	#5,(sp)			; already super?
	beq.b	.1
    lea     6(sp),a0
    tst.w   0x59e.w         ; long stackframes?
    beq.b   .1
    addq.l  #2,a0
.1: move.w	(a0)+,d0		; d0.w is opcode
    cmp.w   #12,d0          ; opcode 12 = midiws
    beq.s   xbios_midiws
    move.l  xbios_old,-(sp) ; jump to old xbios
    rts

